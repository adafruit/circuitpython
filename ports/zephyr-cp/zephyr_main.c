#include <stdio.h>

#include <zephyr/kernel.h>
#include <zephyr/drivers/hwinfo.h>

extern int circuitpython_main(void);

// Note: __has_feature must only be evaluated in a nested #if guarded by
// defined(__has_feature); older GCC versions fail to parse it otherwise.
#if defined(__has_feature)
#if __has_feature(address_sanitizer)
#define CP_HAS_ASAN 1
#endif
#endif
#if defined(__SANITIZE_ADDRESS__) || defined(CP_HAS_ASAN)
// ASAN's stack-use-after-return detection (on by default in recent runtimes)
// moves C locals into "fake stack" frames on the heap. The GC scans the real
// machine stack for live object pointers, so with fake stacks enabled it misses
// pointers held in C frames and collects still-referenced objects, corrupting
// the heap. GCC's instrumentation consults this global at every instrumented
// function entry, so clearing it from a constructor disables only that check;
// all other ASAN checks stay enabled.
extern char __asan_option_detect_stack_use_after_return;
__attribute__((constructor)) static void cp_disable_asan_fake_stack(void) {
    __asan_option_detect_stack_use_after_return = 0;
}
#endif

// The reset cause read and cleared at boot, consumed by
// common-hal/microcontroller (nRF RESETREAS bits are sticky otherwise).
uint32_t cp_reset_cause = 0;

int main(void) {
    // Read and clear the reset cause early: nRF's RESETREAS bits are sticky
    // and would otherwise mask later causes in every subsequent get_reset_cause
    // (e.g. a watchdog reset would keep reporting an older pin reset). Stash
    // the value for get_reset_reason() and print it once as a boot diagnostic.
    if (hwinfo_get_reset_cause(&cp_reset_cause) == 0) {
        hwinfo_clear_reset_cause();
    }
    // Use a unique name for CP main so that the linker needs to look in libcircuitpython.a
    return circuitpython_main();
}
