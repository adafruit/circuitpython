// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2023 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "tusb.h"

#include "py/ringbuf.h"
#include "py/runtime.h"
#include "shared/runtime/interrupt_char.h"
#include "supervisor/usb.h"
#include "supervisor/background_callback.h"
#include "supervisor/shared/serial.h"
#include "supervisor/shared/status_bar.h"
#include "supervisor/shared/tick.h"

#ifndef DEBUG
#define DEBUG (0)
#endif

// Buffer the incoming serial data in the background so that we can look for the
// interrupt character.
static ringbuf_t _incoming_ringbuf;
static uint8_t _buf[16];

static uint8_t _dev_addr;
static uint8_t _interface;

// A keyboard interface that is not boot-protocol, detected at mount time from
// its report descriptor. Composite HID devices (CircuitPython's usb_hid among
// them) present bInterfaceProtocol NONE and prefix each report with a report
// ID, so they fail the boot-protocol check in usb_keyboard_attach().
static uint8_t _report_kbd_dev_addr;
static uint8_t _report_kbd_interface;
static uint8_t _report_kbd_report_id;

#define FLAG_SHIFT (1)
#define FLAG_NUMLOCK (2)
#define FLAG_CTRL (4)
#define FLAG_STRING (8)

static uint8_t user_keymap[384];
static size_t user_keymap_len = 0;

// The TinyUSB callbacks below run in tuh_task(), which may be a task of its own,
// such as the espressif USB host task (CIRCUITPY_USB_HOST_TASK) or the MAX3421E
// task on espressif, while the VM reads keys, repeats them and attaches and
// detaches the keyboard. This lock keeps them apart. Nothing waits for TinyUSB
// while holding it.
#if OSAL_MUTEX_REQUIRED
static osal_mutex_def_t _lock_def;
static osal_mutex_t _lock;
#else
#define _lock NULL
#endif

static void _lock_take(void) {
    (void)osal_mutex_lock(_lock, OSAL_TIMEOUT_WAIT_FOREVER);
}

static void _lock_give(void) {
    (void)osal_mutex_unlock(_lock);
}

void usb_keymap_set(const uint8_t *buf, size_t len) {
    user_keymap_len = len = MIN(len, sizeof(user_keymap));
    memcpy(user_keymap, buf, len);
    memset(user_keymap + len, 0, sizeof(user_keymap) - len);
}

struct keycode_mapper {
    uint8_t first, last, code, flags;
    const char *data;
};

#define SEP "\0" // separator in FLAG_STRING sequences
#define NOTHING "" // in FLAG_STRING sequences
#define CURSOR_UP "\e[A"
#define CURSOR_DOWN "\e[B"
#define CURSOR_LEFT "\e[D"
#define CURSOR_RIGHT "\e[C"
#define CURSOR_PGUP "\e[5~"
#define CURSOR_PGDN "\e[6~"
#define CURSOR_HOME "\e[H"
#define CURSOR_END "\e[F"
#define CURSOR_INS "\e[2~"
#define CURSOR_DEL "\e[3~"

// https://learn.microsoft.com/ru-ru/windows/console/console-virtual-terminal-sequences
// https://aperiodic.net/phil/archives/Geekery/term-function-keys/
// https://www.microfocus.com/documentation/rumba/desktop951/RumbaSystemAdminGuide/GUID-5F92BA7F-107A-4101-B4E7-E0FC73F0CD99.html
// showkey -a
#define F1      "\eOP"
#define F2      "\eOQ"
#define F3      "\eOR"
#define F4      "\eOS"
#define F5      "\e[15~"
#define F6      "\e[17~"
#define F7      "\e[18~"
#define F8      "\e[19~"
#define F9      "\e[20~"
#define F10     "\e[21~"
#define F11     "\e[23~"
#define F12     "\e[24~"

#define PRINT_SCREEN     "\e[i"
#define CTRL_UP "\e[1;5A"
#define CTRL_DOWN "\e[1;5B"
#define CTRL_RIGHT "\e[1;5C"
#define CTRL_LEFT "\e[1;5D"
#define CTRL_PGUP "\e[5;5~"
#define CTRL_PGDN "\e[6;5~"
#define CTRL_HOME "\e[1;5H"
#define CTRL_END "\e[1;5F"
#define CTRL_INS "\e[2;5~"
#define CTRL_DEL "\e[3;5~"
#define CTRL_F1      "\e[1;5P"
#define CTRL_F2      "\e[1;5Q"
#define CTRL_F3      "\e[1;5R"
#define CTRL_F4      "\e[1;5S"
#define CTRL_F5      "\e[15;5~"
#define CTRL_F6      "\e[17;5~"
#define CTRL_F7      "\e[18;5~"
#define CTRL_F8      "\e[19;5~"
#define CTRL_F9      "\e[20;5~"
#define CTRL_F10     "\e[21;5~"
#define CTRL_F11     "\e[23;5~"
#define CTRL_F12     "\e[24;5~"


static struct keycode_mapper keycode_to_ascii[] = {
    { HID_KEY_A, HID_KEY_Z, 'a', 0, NULL},

    { HID_KEY_1, HID_KEY_9, 0, FLAG_SHIFT, "!@#$%^&*()" },
    { HID_KEY_1, HID_KEY_9, '1', 0, },
    { HID_KEY_0, HID_KEY_0, ')', FLAG_SHIFT, },
    { HID_KEY_0, HID_KEY_0, '0', 0, },

    { HID_KEY_ENTER, HID_KEY_ENTER, '\n', FLAG_CTRL },
    { HID_KEY_ENTER, HID_KEY_SLASH, 0, FLAG_SHIFT, "\n\x1b\177\t _+{}|~:\"~<>?" },
    { HID_KEY_ENTER, HID_KEY_SLASH, 0, 0, "\r\x1b\10\t -=[]\\#;'`,./" },

    { HID_KEY_KEYPAD_DIVIDE, HID_KEY_KEYPAD_DECIMAL, 0, FLAG_NUMLOCK | FLAG_STRING,
      "/\0" "*\0" "-\0" "+\0" "\n\0" CURSOR_END SEP CURSOR_DOWN SEP CURSOR_PGDN SEP CURSOR_LEFT SEP NOTHING SEP CURSOR_RIGHT SEP CURSOR_HOME SEP CURSOR_UP SEP CURSOR_PGUP SEP CURSOR_INS SEP CURSOR_DEL},
    { HID_KEY_KEYPAD_DIVIDE, HID_KEY_KEYPAD_DECIMAL, 0, 0, "/*-+\n1234567890." },

    { HID_KEY_INSERT, HID_KEY_ARROW_UP, 0, FLAG_STRING | FLAG_CTRL, CTRL_INS SEP CTRL_HOME SEP CTRL_PGUP SEP CTRL_DEL SEP CTRL_END SEP CTRL_PGDN SEP CTRL_RIGHT SEP CTRL_LEFT SEP CTRL_DOWN SEP CTRL_UP},
    { HID_KEY_INSERT, HID_KEY_ARROW_UP, 0, FLAG_STRING, CURSOR_INS SEP CURSOR_HOME SEP CURSOR_PGUP SEP CURSOR_DEL SEP CURSOR_END SEP CURSOR_PGDN SEP CURSOR_RIGHT SEP CURSOR_LEFT SEP CURSOR_DOWN SEP CURSOR_UP},
    { HID_KEY_F1, HID_KEY_F12, 0, FLAG_STRING | FLAG_CTRL, CTRL_F1 SEP CTRL_F2 SEP CTRL_F3 SEP CTRL_F4 SEP CTRL_F5 SEP CTRL_F6 SEP CTRL_F7 SEP CTRL_F8 SEP CTRL_F9 SEP CTRL_F10 SEP CTRL_F11 SEP CTRL_F12},
    { HID_KEY_F1, HID_KEY_F12, 0, FLAG_STRING, F1 SEP F2 SEP F3 SEP F4 SEP F5 SEP F6 SEP F7 SEP F8 SEP F9 SEP F10 SEP F11 SEP F12},

    { HID_KEY_PAUSE, HID_KEY_PAUSE, 0x1a, 0, },
    { HID_KEY_PRINT_SCREEN, HID_KEY_PRINT_SCREEN, 0, FLAG_STRING, PRINT_SCREEN},

};

static bool report_contains(const hid_keyboard_report_t *report, uint8_t key) {
    for (int i = 0; i < 6; i++) {
        if (report->keycode[i] == key) {
            return true;
        }
    }
    return false;
}

static const char *old_buf = NULL;
static size_t buf_size = 0;
// this matches Linux default of 500ms to first repeat, 1/20s thereafter
enum { initial_repeat_time = 500, default_repeat_time = 50 };
static uint64_t repeat_deadline;
static void repeat_f(void *unused);
background_callback_t repeat_cb = {repeat_f, NULL, NULL, NULL};

static void set_repeat_deadline(uint64_t new_deadline) {
    repeat_deadline = new_deadline;
    background_callback_add_core(&repeat_cb);
}

static void send_bufn_core(const char *buf, size_t n) {
    old_buf = buf;
    buf_size = n;
    // repeat_timeout = millis() + repeat_time;
    for (; n--; buf++) {
        int code = *buf;
        if (code == mp_interrupt_char) {
            ringbuf_clear(&_incoming_ringbuf);
            mp_sched_keyboard_interrupt();
            continue;
        }
        if (ringbuf_num_empty(&_incoming_ringbuf) == 0) {
            // Drop on the floor
            return;
        }
        ringbuf_put(&_incoming_ringbuf, code);
    }
}

static void send_bufn(const char *buf, size_t n) {
    send_bufn_core(buf, n);
    set_repeat_deadline(supervisor_ticks_ms64() + initial_repeat_time);
}

static void send_bufz(const char *buf) {
    send_bufn(buf, strlen(buf));
}

static void send_byte(uint8_t code) {
    static char buf[1];
    buf[0] = code;
    send_bufn(buf, 1);
}

static void send_repeat(void) {
    if (old_buf) {
        uint64_t now = supervisor_ticks_ms64();
        if (now >= repeat_deadline) {
            send_bufn_core(old_buf, buf_size);
            set_repeat_deadline(now + default_repeat_time);
        } else {
            background_callback_add_core(&repeat_cb);
        }
    }
}

static void repeat_f(void *unused) {
    _lock_take();
    send_repeat();
    _lock_give();
}

hid_keyboard_report_t old_report;

static const char *skip_nuls(const char *buf, size_t n) {
    while (n--) {
        buf += strlen(buf) + 1;
    }
    return buf;
}

static void process_event(uint8_t dev_addr, uint8_t instance, const hid_keyboard_report_t *report) {
    bool has_altgr = (user_keymap_len > 256);
    bool altgr = has_altgr && report->modifier & 0x40;
    bool alt = has_altgr ? report->modifier & 0x4 : report->modifier & 0x44;
    bool shift = report->modifier & 0x22;
    bool ctrl = report->modifier & 0x11;
    bool caps = old_report.reserved & 1;
    bool num = old_report.reserved & 2;
    uint8_t code = 0;

    if (report->keycode[0] == 1 && report->keycode[1] == 1) {
        // keyboard says it has exceeded max kro
        return;
    }

    // something was pressed or released, so cancel any key repeat
    old_buf = NULL;

    for (int i = 0; i < 6; i++) {
        uint8_t keycode = report->keycode[i];
        if (keycode == 0) {
            continue;
        }
        if (report_contains(&old_report, keycode)) {
            continue;
        }

        /* key is newly pressed */
        if (keycode == HID_KEY_NUM_LOCK) {
            num = !num;
        } else if (keycode == HID_KEY_CAPS_LOCK) {
            caps = !caps;
        } else {
            size_t idx = keycode + (altgr ? 256 : shift ? 128 : 0);
            uint8_t ascii = user_keymap[idx];
            #if DEBUG
            mp_printf(&mp_plat_print, "lookup HID keycode %d mod %x at idx %d -> ascii %d (%c)\n",
                keycode, report->modifier, idx, ascii, ascii >= 32 && ascii <= 126 ? ascii : '.');
            #endif
            if (ascii != 0) {
                if (ctrl) {
                    ascii &= 0x1f;
                } else if (ascii >= 'a' && ascii <= 'z' && caps) {
                    ascii ^= ('a' ^ 'A');
                }
                send_byte(ascii);
                continue;
            }

            for (size_t j = 0; j < MP_ARRAY_SIZE(keycode_to_ascii); j++) {
                struct keycode_mapper *mapper = &keycode_to_ascii[j];
                if (!(keycode >= mapper->first && keycode <= mapper->last)) {
                    continue;
                }
                if (mapper->flags & FLAG_SHIFT && !shift) {
                    continue;
                }
                if (mapper->flags & FLAG_NUMLOCK && !num) {
                    continue;
                }
                if (mapper->flags & FLAG_CTRL && !ctrl) {
                    continue;
                }
                if (mapper->flags & FLAG_STRING) {
                    const char *msg = skip_nuls(mapper->data, keycode - mapper->first);
                    send_bufz(msg);
                    break;
                } else if (mapper->data) {
                    code = mapper->data[keycode - mapper->first];
                } else {
                    code = keycode - mapper->first + mapper->code;
                }
                if (code >= 'a' && code <= 'z' && (shift ^ caps)) {
                    code ^= ('a' ^ 'A');
                }
                if (ctrl) {
                    code &= 0x1f;
                }
                if (alt) {
                    code ^= 0x80;
                }
                send_byte(code);
                break;
            }
        }
    }

    uint8_t leds = (caps | (num << 1));
    if (leds != old_report.reserved) {
        // A report-protocol keyboard needs its report ID on the LED output report too.
        uint8_t report_id = 0;
        if (dev_addr == _report_kbd_dev_addr && instance == _report_kbd_interface) {
            report_id = _report_kbd_report_id;
        }
        tuh_hid_set_report(dev_addr, instance /*idx*/, report_id, HID_REPORT_TYPE_OUTPUT /*report_type*/, &leds, sizeof(leds));
    }
    old_report = *report;
    old_report.reserved = leds;
}

bool usb_keyboard_in_use(uint8_t dev_addr, uint8_t interface) {
    return _dev_addr == dev_addr && _interface == interface;
}

void usb_keyboard_detach(uint8_t dev_addr, uint8_t interface) {
    _lock_take();
    if (!usb_keyboard_in_use(dev_addr, interface)) {
        _lock_give();
        return;
    }
    // No more key repeats
    old_buf = NULL;

    tuh_hid_receive_abort(dev_addr, interface);
    _dev_addr = 0;
    _interface = 0;
    _lock_give();
    supervisor_status_bar_request_update(false);
}

void usb_keyboard_attach(uint8_t dev_addr, uint8_t interface) {
    _lock_take();
    if (usb_keyboard_in_use(dev_addr, interface) || _dev_addr != 0) {
        _lock_give();
        return;
    }
    uint8_t const itf_protocol = tuh_hid_interface_protocol(dev_addr, interface);
    bool is_boot_keyboard = itf_protocol == HID_ITF_PROTOCOL_KEYBOARD;
    // Detected at mount time by tuh_hid_mount_cb(). Device addresses start at
    // 1, so a zeroed _report_kbd_dev_addr never matches.
    bool is_report_keyboard = dev_addr == _report_kbd_dev_addr && interface == _report_kbd_interface;
    if (is_boot_keyboard || is_report_keyboard) {
        _dev_addr = dev_addr;
        _interface = interface;
        tuh_hid_receive_report(dev_addr, interface);
    }
    _lock_give();
    supervisor_status_bar_request_update(false);
}

void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t interface, uint8_t const *desc_report, uint16_t desc_len) {
    // A non-boot interface can still be a keyboard, using report protocol.
    // Find keyboard usage in its report descriptor so that we poll it too.
    // Otherwise its interrupt IN endpoint is never drained, and a CircuitPython
    // device plugged into us raises OSError("USB busy") on its second
    // send_report() (#10721).
    _lock_take();
    if (tuh_hid_interface_protocol(dev_addr, interface) == HID_ITF_PROTOCOL_NONE &&
        _report_kbd_dev_addr == 0) {
        tuh_hid_report_info_t report_info[8];
        uint8_t report_count =
            tuh_hid_parse_report_descriptor(report_info, TU_ARRAY_SIZE(report_info), desc_report, desc_len);
        for (uint8_t i = 0; i < report_count; i++) {
            if (report_info[i].usage_page == HID_USAGE_PAGE_DESKTOP &&
                report_info[i].usage == HID_USAGE_DESKTOP_KEYBOARD) {
                _report_kbd_dev_addr = dev_addr;
                _report_kbd_interface = interface;
                _report_kbd_report_id = report_info[i].report_id;
                break;
            }
        }
    }
    _lock_give();
    usb_keyboard_attach(dev_addr, interface);
}

void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t interface) {
    _lock_take();
    if (dev_addr == _report_kbd_dev_addr && interface == _report_kbd_interface) {
        _report_kbd_dev_addr = 0;
        _report_kbd_interface = 0;
        _report_kbd_report_id = 0;
    }
    _lock_give();
    usb_keyboard_detach(dev_addr, interface);
}

void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const *report, uint16_t len) {
    _lock_take();
    if (len == sizeof(hid_keyboard_report_t)) {
        process_event(dev_addr, instance, (hid_keyboard_report_t *)report);
    } else if (dev_addr == _report_kbd_dev_addr && instance == _report_kbd_interface &&
               _report_kbd_report_id != 0 &&
               len == sizeof(hid_keyboard_report_t) + 1 &&
               report[0] == _report_kbd_report_id) {
        // Report-protocol keyboards prefix the report with its report ID.
        process_event(dev_addr, instance, (const hid_keyboard_report_t *)(report + 1));
    }
    _lock_give();
    // Always request the next report, even when this one wasn't a keyboard
    // report we understand (a mouse or consumer control report ID from a
    // composite device, for instance), so that the device's endpoint keeps
    // draining and polling doesn't stop.
    tuh_hid_receive_report(dev_addr, instance);
}

void usb_keyboard_init(void) {
    #if OSAL_MUTEX_REQUIRED
    // This runs once, at startup.
    _lock = osal_mutex_create(&_lock_def);
    #endif
    _lock_take();
    ringbuf_init(&_incoming_ringbuf, _buf, sizeof(_buf));
    _lock_give();
}

uint32_t usb_keyboard_chars_available(void) {
    _lock_take();
    uint32_t count = ringbuf_num_filled(&_incoming_ringbuf);
    _lock_give();
    return count;
}

char usb_keyboard_read_char(void) {
    char c = -1;
    _lock_take();
    if (ringbuf_num_filled(&_incoming_ringbuf) > 0) {
        c = ringbuf_get(&_incoming_ringbuf);
    }
    _lock_give();
    return c;
}

void usb_keyboard_status(void) {
    if (_dev_addr != 0 && _interface != 0) {
        serial_write("🖮");
    }
}
