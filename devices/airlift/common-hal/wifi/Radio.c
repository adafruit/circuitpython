// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdio.h>
#include <string.h>

#include "common-hal/wifi/__init__.h"
#include "shared/runtime/interrupt_char.h"
#include "py/gc.h"
#include "py/mphal.h"
#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/ipaddress/IPv4Address.h"
#include "shared-bindings/ipaddress/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/time/__init__.h"
#include "shared-bindings/wifi/AuthMode.h"
#include "shared-bindings/wifi/Network.h"
#include "shared-bindings/wifi/Radio.h"
#include "shared-bindings/wifi/ScannedNetworks.h"

#include "common-hal/socketpool/__init__.h"


// Print out SPI traffic with AirLift.
#define DEBUG_AIRLIFT 0

#if DEBUG_AIRLIFT
static const char *command_name(uint8_t command) {
    switch (command) {
        case START_CMD:
            return "START_CMD";
        case END_CMD:
            return "END_CMD";
        case ERR_CMD:
            return "ERR_CMD";
        case REPLY_FLAG:
            return "REPLY_FLAG";
        case CMD_FLAG:
            return "CMD_FLAG";
        case SET_NET_CMD:
            return "SET_NET_CMD";
        case SET_PASSPHRASE_CMD:
            return "SET_PASSPHRASE_CMD";
        case SET_KEY_CMD:
            return "SET_KEY_CMD";
        // case TEST_CMD: return "TEST_CMD"; // Not implemented
        case SET_IP_CONFIG:
            return "SET_IP_CONFIG";
        case SET_DNS_CONFIG:
            return "SET_DNS_CONFIG";
        case SET_HOSTNAME:
            return "SET_HOSTNAME";
        case SET_POWER_MODE_CMD:
            return "SET_POWER_MODE_CMD";
        case SET_AP_NET_CMD:
            return "SET_AP_NET_CMD";
        case SET_AP_PASSPHRASE_CMD:
            return "SET_AP_PASSPHRASE_CMD";
        case SET_DEBUG_CMD:
            return "SET_DEBUG_CMD";
        case GET_TEMPERATURE_CMD:
            return "GET_TEMPERATURE_CMD";
        case GET_DNS_CONFIG_CMD:
            return "GET_DNS_CONFIG_CMD";
        case GET_REASON_CODE_CMD:
            return "GET_REASON_CODE_CMD";
        case GET_CONN_STATUS_CMD:
            return "GET_CONN_STATUS_CMD";
        case GET_IPADDR_CMD:
            return "GET_IPADDR_CMD";
        case GET_MACADDR_CMD:
            return "GET_MACADDR_CMD";
        case GET_CURR_SSID_CMD:
            return "GET_CURR_SSID_CMD";
        case GET_CURR_BSSID_CMD:
            return "GET_CURR_BSSID_CMD";
        case GET_CURR_RSSI_CMD:
            return "GET_CURR_RSSI_CMD";
        case GET_CURR_ENCT_CMD:
            return "GET_CURR_ENCT_CMD";
        case SCAN_NETWORKS:
            return "SCAN_NETWORKS";
        case START_SERVER_TCP_CMD:
            return "START_SERVER_TCP_CMD";
        case GET_STATE_TCP_CMD:
            return "GET_STATE_TCP_CMD";
        case DATA_SENT_TCP_CMD:
            return "DATA_SENT_TCP_CMD";
        case AVAIL_DATA_TCP_CMD:
            return "AVAIL_DATA_TCP_CMD";
        case GET_DATA_TCP_CMD:
            return "GET_DATA_TCP_CMD";
        case START_CLIENT_TCP_CMD:
            return "START_CLIENT_TCP_CMD";
        case STOP_CLIENT_TCP_CMD:
            return "STOP_CLIENT_TCP_CMD";
        case GET_CLIENT_STATE_TCP_CMD:
            return "GET_CLIENT_STATE_TCP_CMD";
        case DISCONNECT_CMD:
            return "DISCONNECT_CMD";
        // case GET_IDX_SSID_CMD: return "GET_IDX_SSID_CMD"; // Not implemented.
        case GET_IDX_RSSI_CMD:
            return "GET_IDX_RSSI_CMD";
        case GET_IDX_ENCT_CMD:
            return "GET_IDX_ENCT_CMD";
        case REQ_HOST_BY_NAME_CMD:
            return "REQ_HOST_BY_NAME_CMD";
        case GET_HOST_BY_NAME_CMD:
            return "GET_HOST_BY_NAME_CMD";
        case START_SCAN_NETWORKS:
            return "START_SCAN_NETWORKS";
        case GET_FW_VERSION_CMD:
            return "GET_FW_VERSION_CMD";
        // case GET_TEST_CMD: return "GET_TEST_CMD"; // Not implemented.
        case SEND_UDP_DATA_CMD:
            return "SEND_UDP_DATA_CMD";
        case GET_REMOTE_DATA_CMD:
            return "GET_REMOTE_DATA_CMD";
        case GET_TIME:
            return "GET_TIME";
        case GET_IDX_BSSID_CMD:
            return "GET_IDX_BSSID_CMD";
        case GET_IDX_CHAN_CMD:
            return "GET_IDX_CHAN_CMD";
        case PING_CMD:
            return "PING_CMD";
        case GET_SOCKET_CMD:
            return "GET_SOCKET_CMD";
        case SET_CLI_CERT:
            return "SET_CLI_CERT";
        case SET_PK:
            return "SET_PK";
        case SEND_DATA_TCP_CMD:
            return "SEND_DATA_TCP_CMD";
        case GET_DATABUF_TCP_CMD:
            return "GET_DATABUF_TCP_CMD";
        case INSERT_DATABUF_TCP_CMD:
            return "INSERT_DATABUF_TCP_CMD";
        case SET_ENT_IDENT_CMD:
            return "SET_ENT_IDENT_CMD";
        case SET_ENT_UNAME_CMD:
            return "SET_ENT_UNAME_CMD";
        case SET_ENT_PASSWD_CMD:
            return "SET_ENT_PASSWD_CMD";
        case SET_ENT_ENABLE_CMD:
            return "SET_ENT_ENABLE_CMD";
        case SET_DIGITAL_WRITE_CMD:
            return "SET_DIGITAL_WRITE_CMD";
        case SET_ANALOG_WRITE_CMD:
            return "SET_ANALOG_WRITE_CMD";
        case SET_DIGITAL_READ_CMD:
            return "SET_DIGITAL_READ_CMD";
        case SET_ANALOG_READ_CMD:
            return "SET_ANALOG_READ_CMD";
        case WRITE_FILE:
            return "WRITE_FILE";
        case READ_FILE:
            return "READ_FILE";
        case DELETE_FILE:
            return "DELETE_FILE";
        case EXISTS_FILE:
            return "EXISTS_FILE";
        case DOWNLOAD_FILE:
            return "DOWNLOAD_FILE";
        case APPLY_OTA_COMMAND:
            return "APPLY_OTA_COMMAND";
        case RENAME_FILE:
            return "RENAME_FILE";
        case DOWNLOAD_OTA:
            return "DOWNLOAD_OTA";
        case SOCKET_SOCKET_CMD:
            return "SOCKET_SOCKET_CMD";
        case SOCKET_CLOSE_CMD:
            return "SOCKET_CLOSE_CMD";
        case SOCKET_ERRNO_CMD:
            return "SOCKET_ERRNO_CMD";
        case SOCKET_BIND_CMD:
            return "SOCKET_BIND_CMD";
        case SOCKET_LISTEN_CMD:
            return "SOCKET_LISTEN_CMD";
        case SOCKET_ACCEPT_CMD:
            return "SOCKET_ACCEPT_CMD";
        case SOCKET_CONNECT_CMD:
            return "SOCKET_CONNECT_CMD";
        case SOCKET_SEND_CMD:
            return "SOCKET_SEND_CMD";
        case SOCKET_RECV_CMD:
            return "SOCKET_RECV_CMD";
        case SOCKET_SENDTO_CMD:
            return "SOCKET_SENDTO_CMD";
        case SOCKET_RECVFROM_CMD:
            return "SOCKET_RECVFROM_CMD";
        case SOCKET_IOCTL_CMD:
            return "SOCKET_IOCTL_CMD";
        case SOCKET_POLL_CMD:
            return "SOCKET_POLL_CMD";
        case SOCKET_SETSOCKOPT_CMD:
            return "SOCKET_SETSOCKOPT_CMD";
        case SOCKET_GETSOCKOPT_CMD:
            return "SOCKET_GETSOCKOPT_CMD";
        case SOCKET_GETPEERNAME_CMD:
            return "SOCKET_GETPEERNAME_CMD";
        default:
            return "*UNKNOWN COMMAND*";
    }
}
#endif // DEBUG_AIRLIFT

// Release CS and lock if held.
static void spi_end_transaction(wifi_radio_obj_t *self) {
    if (common_hal_busio_spi_has_lock(self->spi)) {
        common_hal_digitalio_digitalinout_set_value(self->cs_dio, true);
        common_hal_busio_spi_unlock(self->spi);
    }
}

// Wait for ready pin to become low or high.
// Terminate the spi transaction if in progress, and raise an exception if a timeout is exceeded.
//
static void wait_for_ready_raise(wifi_radio_obj_t *self, bool value, uint32_t timeout_ms) {
    uint64_t start = mp_hal_ticks_ms();
    while ((mp_hal_ticks_ms() - start) < timeout_ms) {
        if (common_hal_digitalio_digitalinout_get_value(self->ready_dio) == value) {
            return;
        }
        RUN_BACKGROUND_TASKS;
        if (mp_hal_is_interrupted()) {
            break;
        }
    }

    spi_end_transaction(self);
    mp_raise_msg_varg(&mp_type_TimeoutError, MP_ERROR_TEXT("timeout waiting for ready %q"),
        value ? MP_QSTR_True : MP_QSTR_False);
}

// Wait for co-processor to be ready, then grab lock and CS.
static bool spi_begin_transaction(wifi_radio_obj_t *self, uint32_t timeout_ms) {
    // SPI might have been deinited out from under us..
    if (!self->spi || common_hal_busio_spi_deinited(self->spi)) {
        return false;
    }
    // The ready line is set low when the NINA firmware is ready to start an SPI transaction.
    // Once CS is set low to signal an SPI transaction has started, NINA sets the ready line high
    // to indicate it has seen the CS transition to low.
    // Wait for the previous command to complete.
    wait_for_ready_raise(self, false, timeout_ms);

    while (!common_hal_busio_spi_try_lock(self->spi)) {
        RUN_BACKGROUND_TASKS;
        if (mp_hal_is_interrupted()) {
            break;
        }
    }
    common_hal_busio_spi_configure(self->spi, 8000000, 0, 0, 8);

    common_hal_digitalio_digitalinout_set_value(self->cs_dio, false);
    // This is not a variable timeout, because we are not waiting for a command to complete.
    wait_for_ready_raise(self, true, AIRLIFT_DEFAULT_TIMEOUT_MS);
    return true;
}

static void check_for_enabled(wifi_radio_obj_t *self) {
    if (!common_hal_wifi_radio_get_enabled(self)) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("WiFi is not enabled"));
    }
}

// AirLift communication routines, some of which are also called from other classes.

// Read byte_length bytes from SPI.
static void wifi_radio_read(wifi_radio_obj_t *self, uint8_t *bytes, size_t byte_length) {
    if (!common_hal_busio_spi_read(self->spi, bytes, byte_length, 0xFF)) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Failed"));
    }
}

// Read just one byte from SPI.
static uint8_t wifi_radio_read_byte(wifi_radio_obj_t *self) {
    uint8_t one_byte;
    if (common_hal_busio_spi_read(self->spi, &one_byte, 1, 0xFF)) {
        return one_byte;
    } else {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Failed"));
    }
}

// Send just one byte from SPI
static bool wifi_radio_write_byte(wifi_radio_obj_t *self, uint8_t b) {
    return common_hal_busio_spi_write(self->spi, &b, 1);
}

static bool wifi_radio_write(wifi_radio_obj_t *self, const uint8_t *bytes, size_t byte_length) {
    return common_hal_busio_spi_write(self->spi, bytes, byte_length);
}

// Wait for specific byte
static void wifi_radio_wait_spi_char(wifi_radio_obj_t *self, uint8_t desired) {
    for (int i = 0; i < 10; i++) {
        uint8_t r = wifi_radio_read_byte(self);
        if (r == ERR_CMD) {
            spi_end_transaction(self);
            mp_raise_RuntimeError(MP_ERROR_TEXT("Error response to AirLift command"));
        }
        if (r == desired) {
            return;
        }
        mp_hal_delay_ms(10);
    }
    spi_end_transaction(self);
    mp_raise_RuntimeError(MP_ERROR_TEXT("timeout waiting for byte"));
}

// Check that next byte matches expected value.
static void wifi_radio_read_and_check_byte(wifi_radio_obj_t *self, uint8_t desired) {
    uint8_t r = wifi_radio_read_byte(self);
    if (r != desired) {
        spi_end_transaction(self);
        mp_raise_RuntimeError_varg(MP_ERROR_TEXT("Expected %02x but got %02x"), desired, r);
    }
}

// Send command via SPI.
bool wifi_radio_send_command(wifi_radio_obj_t *self, uint8_t cmd,
    const uint8_t **params, const size_t *param_lengths, lengths_size_t param_lengths_size, size_t num_params) {

    if (!spi_begin_transaction(self, AIRLIFT_DEFAULT_TIMEOUT_MS)) {
        return false;
    }

    // Send command opcode and number of params.
    const uint8_t command_begin[] = {
        START_CMD,
        cmd & ~REPLY_FLAG,
        num_params,
    };
    wifi_radio_write(self, command_begin, sizeof(command_begin));
    size_t num_bytes_sent = sizeof(command_begin);

    #if DEBUG_AIRLIFT
    PLAT_PRINTF(">cmd %s (%02x)\n", command_name(cmd), cmd);
    #endif // DEBUG_AIRLIFT

    for (size_t i = 0; i < num_params; i++) {
        switch (param_lengths_size) {
            case LENGTHS_16:
                // Length is sent big-endian.
                wifi_radio_write_byte(self, (param_lengths[i] >> 8) & 0xFF);
                num_bytes_sent++;
                [[fallthrough]];
            case LENGTHS_8:
                wifi_radio_write_byte(self, param_lengths[i] & 0xFF);
                num_bytes_sent++;
                break;
        }

        wifi_radio_write(self, params[i], param_lengths[i]);
        num_bytes_sent += param_lengths[i];

        #if DEBUG_AIRLIFT
        PLAT_PRINTF(">>param #%d (length: %d) --x", i, param_lengths[i]);
        for (size_t j = 0; j < param_lengths[i]; j++) {
            PLAT_PRINTF(" %02x", params[i][j]);
        }
        PLAT_PRINTF(" --d");
        for (size_t j = 0; j < param_lengths[i]; j++) {
            PLAT_PRINTF(" %d", params[i][j]);
        }
        PLAT_PRINTF(" --c \"");
        for (size_t j = 0; j < param_lengths[i]; j++) {
            PLAT_PRINTF("%c", params[i][j]);
        }
        PLAT_PRINTF("\"\n");
        #endif // DEBUG_AIRLIFT
    }

    while (num_bytes_sent++ % 4 != 0) {
        wifi_radio_write_byte(self, 0x00);
    }

    spi_end_transaction(self);
    return true;
}

// Wait for and parse response
size_t wifi_radio_wait_response_cmd(wifi_radio_obj_t *self, uint8_t cmd,
    uint8_t **responses, size_t *response_lengths, lengths_size_t response_lengths_size, size_t max_responses,
    uint32_t response_timeout_ms) {
    spi_begin_transaction(self, response_timeout_ms);

    wifi_radio_wait_spi_char(self, START_CMD);
    wifi_radio_read_and_check_byte(self, cmd | REPLY_FLAG);
    uint8_t num_responses = wifi_radio_read_byte(self);

    if (num_responses > max_responses) {
        num_responses = max_responses;
    }

    if (num_responses == 0) {
        PLAT_PRINTF("<zero responses\n\n");
    }
    for (size_t i = 0; i < num_responses; i++) {
        size_t response_length = wifi_radio_read_byte(self);
        // Two-byte response lengths are big-endian.
        if (response_lengths_size == LENGTHS_16) {
            response_length = (response_length << 8) | wifi_radio_read_byte(self);
        }
        // Don't overflow the supplied buffer.
        size_t read_length = MIN(response_length, response_lengths[i]);
        wifi_radio_read(self, responses[i], read_length);
        // Update the passed-in length with what was actually read, so the caller knows how many bytes
        // were read for each response.
        response_lengths[i] = read_length;

        // Read and discard bytes that didn't fit in buffer.
        if (read_length > response_lengths[i]) {
            #if DEBUG_AIRLIFT
            PLAT_PRINTF("<!! response %d too long. expected %d, got %d\n", i, response_lengths[i], response_length);
            #endif
            while (read_length-- > 0) {
                wifi_radio_read_byte(self);
            }
        }

        #if DEBUG_AIRLIFT
        PLAT_PRINTF("<response #%d (length %d) --x", i, response_length);
        for (size_t j = 0; j < response_length; j++) {
            PLAT_PRINTF(" %02x", responses[i][j]);
        }
        PLAT_PRINTF(" --d");
        for (size_t j = 0; j < response_length; j++) {
            PLAT_PRINTF(" %d", responses[i][j]);
        }
        PLAT_PRINTF(" --c \"");
        for (size_t j = 0; j < response_length; j++) {
            PLAT_PRINTF("%c", responses[i][j]);
        }
        PLAT_PRINTF("\"\n\n");
        #endif // DEBUG_AIRLIFT
    }

    wifi_radio_read_and_check_byte(self, END_CMD);

    spi_end_transaction(self);

    return num_responses;
}

size_t wifi_radio_send_command_get_response(wifi_radio_obj_t *self, uint8_t cmd,
    const uint8_t **params, const size_t *param_lengths, lengths_size_t param_lengths_size, size_t num_params,
    uint8_t **responses, size_t *response_lengths, lengths_size_t response_lengths_size, size_t max_responses,
    uint32_t response_timeout_ms) {
    if (!wifi_radio_send_command(self, cmd,
        params, param_lengths, param_lengths_size, num_params)) {
        return 0;
    }
    return wifi_radio_wait_response_cmd(self, cmd,
        responses, response_lengths, response_lengths_size, max_responses,
        response_timeout_ms);
}


static bool wifi_radio_get_mac_address(wifi_radio_obj_t *self, uint8_t mac[MAC_ADDRESS_LENGTH]) {
    const uint8_t ignored = 0xFF;
    const uint8_t *params[1] = { &ignored };
    size_t param_lengths[1] = { 1 };

    uint8_t *responses[1] = { mac };
    size_t response_lengths[1] = { MAC_ADDRESS_LENGTH };

    size_t num_responses = wifi_radio_send_command_get_response(self, GET_MACADDR_CMD,
        params, param_lengths, LENGTHS_8, MP_ARRAY_SIZE(params),
        responses, response_lengths, LENGTHS_8, MP_ARRAY_SIZE(responses),
        AIRLIFT_DEFAULT_TIMEOUT_MS);

    if (num_responses > 0 && response_lengths[0] >= MAC_ADDRESS_LENGTH) {
        return true;
    } else {
        memset(mac, 0, MAC_ADDRESS_LENGTH);
        return false;
    }
}

static bool wifi_radio_get_address_info(wifi_radio_obj_t *self,
    uint8_t ipv4[IPV4_LENGTH], uint8_t netmask[IPV4_LENGTH], uint8_t gateway[IPV4_LENGTH]) {
    const uint8_t ignored = 0xFF;
    const uint8_t *params[1] = { &ignored };
    size_t param_lengths[1] = { 1 };

    uint8_t *responses[3] = { ipv4, netmask, gateway };
    size_t response_lengths[3] = { IPV4_LENGTH, IPV4_LENGTH, IPV4_LENGTH };

    size_t num_responses = wifi_radio_send_command_get_response(self, GET_IPADDR_CMD,
        params, param_lengths, LENGTHS_8, MP_ARRAY_SIZE(params),
        responses, response_lengths, LENGTHS_8, MP_ARRAY_SIZE(responses),
        AIRLIFT_DEFAULT_TIMEOUT_MS);

    if (num_responses == 3) {
        return true;
    } else {
        return false;
    }
}

// Fetch up to two DNS addresses, into a passed C array of IPv4Address objects.
static void wifi_radio_get_dns_config(wifi_radio_obj_t *self, mp_obj_t dns_ipv4_addresses[], size_t *num_dns) {
    const uint8_t ignored = 0xFF;
    const uint8_t *params[1] = { &ignored };
    size_t param_lengths[1] = { 1 };

    uint8_t dns_ipv4[2][IPV4_LENGTH] = { 0 };
    uint8_t *responses[2] = { dns_ipv4[0], dns_ipv4[1] };
    size_t response_lengths[2] = { IPV4_LENGTH, IPV4_LENGTH };

    size_t num_responses = wifi_radio_send_command_get_response(self, GET_DNS_CONFIG_CMD,
        params, param_lengths, LENGTHS_8, MP_ARRAY_SIZE(params),
        responses, response_lengths, LENGTHS_8, MP_ARRAY_SIZE(responses),
        AIRLIFT_DEFAULT_TIMEOUT_MS);

    if (num_responses == 2) {
        for (size_t dns_i = 0; dns_i < 2; dns_i++) {
            // Only count the non-zero IP addresses.
            for (size_t octet_i = 0; octet_i < IPV4_LENGTH; octet_i++) {
                if (dns_ipv4[dns_i][octet_i] != 0) {
                    (*num_dns)++;
                    dns_ipv4_addresses[dns_i] = common_hal_ipaddress_new_ipv4address_from_bytes(dns_ipv4[dns_i]);
                    break;
                }
            }
        }
    }
}

static void wifi_radio_reset(wifi_radio_obj_t *self) {
    if (self->gpio0_dio) {
        common_hal_digitalio_digitalinout_switch_to_output(self->gpio0_dio, true, DRIVE_MODE_PUSH_PULL);
    }

    common_hal_digitalio_digitalinout_set_value(self->cs_dio, true);
    common_hal_digitalio_digitalinout_set_value(self->reset_dio, false);
    mp_hal_delay_ms(10);
    common_hal_digitalio_digitalinout_set_value(self->reset_dio, true);
    mp_hal_delay_ms(750);  // Wait for Airlift to boot.

    if (self->gpio0_dio) {
        common_hal_digitalio_digitalinout_switch_to_input(self->gpio0_dio, PULL_NONE);
    }
}

wl_status_t wifi_radio_get_connection_status(wifi_radio_obj_t *self) {
    uint8_t connection_status;

    uint8_t *responses[1] = { &connection_status };
    size_t response_lengths[1] = { 1 };

    size_t num_responses = wifi_radio_send_command_get_response(self, GET_CONN_STATUS_CMD,
        NULL, NULL, LENGTHS_8, 0,
        responses, response_lengths, LENGTHS_8, MP_ARRAY_SIZE(responses),
        AIRLIFT_DEFAULT_TIMEOUT_MS);

    if (num_responses > 0) {
        return connection_status;
    } else {
        return WL_NO_SHIELD;
    }
}

//////////////////////////////////////////////////////////////////////////////
// common-hal routines called from shared-bindings/Radio.c

bool common_hal_wifi_radio_deinited(wifi_radio_obj_t *self) {
    return self->spi == NULL;
}

void common_hal_wifi_radio_mark_deinit(wifi_radio_obj_t *self) {
    self->enabled = false;
    self->spi = NULL;
}

void common_hal_wifi_radio_deinit(wifi_radio_obj_t *self) {
    if (common_hal_wifi_radio_deinited(self)) {
        return;
    }
    common_hal_wifi_radio_mark_deinit(self);
}

void common_hal_wifi_radio_init_airlift(
    wifi_radio_obj_t *self,
    busio_spi_obj_t *spi,
    digitalio_digitalinout_obj_t *cs,
    digitalio_digitalinout_obj_t *ready,
    digitalio_digitalinout_obj_t *reset,
    digitalio_digitalinout_obj_t *gpio0) {

    self->spi = spi;
    self->cs_dio = cs;
    self->ready_dio = ready;
    self->reset_dio = reset;
    self->gpio0_dio = (gpio0 != mp_const_none) ? gpio0 : NULL;
    self->tls_socket = -1;
    self->hostname = MP_OBJ_NEW_QSTR(MP_QSTR_);
    // Default is not to minimize power.
    self->power_management = POWER_MANAGEMENT_NONE;

    common_hal_digitalio_digitalinout_switch_to_output(self->cs_dio, true, DRIVE_MODE_PUSH_PULL);
    common_hal_digitalio_digitalinout_switch_to_input(self->ready_dio, PULL_NONE);
    common_hal_digitalio_digitalinout_switch_to_output(self->reset_dio, true, DRIVE_MODE_PUSH_PULL);

    if (self->gpio0_dio) {
        common_hal_digitalio_digitalinout_switch_to_input(self->gpio0_dio, PULL_NONE);
    }

    // Perform initial reset
    wifi_radio_reset(self);
    self->enabled = true;

    // There is no NINA-FW command to get the hostname, so set a default hostname to begin with.
    // NINA-FW sets one, but it includes the co-processor chip type, and we don't know the chip type,
    // so instead set a more general name. The partial mac address suffix is the same.

    uint8_t mac[MAC_ADDRESS_LENGTH];
    if (wifi_radio_get_mac_address(self, mac)) {
        char default_hostname[32];
        snprintf(default_hostname, sizeof(default_hostname), "AirLift-%02X%02X%02X", mac[3], mac[4], mac[5]);
        common_hal_wifi_radio_set_hostname(self, default_hostname);
    }
}

mp_obj_t common_hal_wifi_radio_get_version(wifi_radio_obj_t *self) {
    check_for_enabled(self);

    uint8_t firmware_version[AIRLIFT_MAX_FIRMWARE_VERSION_LENGTH];
    uint8_t *responses[1] = { firmware_version };
    size_t response_lengths[1] = { AIRLIFT_MAX_FIRMWARE_VERSION_LENGTH };

    size_t num_responses = wifi_radio_send_command_get_response(self, GET_FW_VERSION_CMD,
        NULL, NULL, LENGTHS_8, 0,
        responses, response_lengths, LENGTHS_8, MP_ARRAY_SIZE(responses),
        AIRLIFT_DEFAULT_TIMEOUT_MS);

    if (num_responses > 0) {
        return mp_obj_new_str((char *)firmware_version, response_lengths[0]);
    } else {
        return mp_const_empty_bytes;
    }
}


bool common_hal_wifi_radio_get_enabled(wifi_radio_obj_t *self) {
    return self->enabled;
}

void common_hal_wifi_radio_set_enabled(wifi_radio_obj_t *self, bool enabled) {
    if (enabled != self->enabled) {
        if (enabled) {
            // Do a fresh reset to disable anything going on now.
        }
        self->enabled = enabled;
    }
}

mp_obj_t common_hal_wifi_radio_get_hostname(wifi_radio_obj_t *self) {
    check_for_enabled(self);

    return self->hostname;
}

void common_hal_wifi_radio_set_hostname(wifi_radio_obj_t *self, const char *hostname) {
    const uint8_t *params[1] = { (const uint8_t *)hostname };
    size_t param_lengths[1] = { strlen(hostname) };

    uint8_t result = 0;
    uint8_t *responses[1] = { &result };
    size_t response_lengths[1] = { 1 };

    size_t num_responses = wifi_radio_send_command_get_response(self, SET_HOSTNAME,
        params, param_lengths, LENGTHS_8, MP_ARRAY_SIZE(params),
        responses, response_lengths, LENGTHS_8, MP_ARRAY_SIZE(responses),
        AIRLIFT_DEFAULT_TIMEOUT_MS);

    if (num_responses == 0 || result != 1) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Failed"));
    }

    // Remember this so we can return it when asked.
    self->hostname = mp_obj_new_str_from_cstr(hostname);
}

mp_obj_t common_hal_wifi_radio_get_mac_address(wifi_radio_obj_t *self) {
    check_for_enabled(self);

    uint8_t mac[MAC_ADDRESS_LENGTH];
    if (wifi_radio_get_mac_address(self, mac)) {
        return mp_obj_new_bytes(mac, MAC_ADDRESS_LENGTH);
    } else {
        return mp_const_none;
    }
}

void common_hal_wifi_radio_set_mac_address(wifi_radio_obj_t *self, const uint8_t *mac) {
    mp_raise_NotImplementedError(NULL);
}

mp_float_t common_hal_wifi_radio_get_tx_power(wifi_radio_obj_t *self) {
    return 20.0f;
}

void common_hal_wifi_radio_set_tx_power(wifi_radio_obj_t *self, const mp_float_t tx_power) {
    mp_raise_NotImplementedError(NULL);
}

wifi_power_management_t common_hal_wifi_radio_get_power_management(wifi_radio_obj_t *self) {
    check_for_enabled(self);

    return self->power_management;
}

void common_hal_wifi_radio_set_power_management(wifi_radio_obj_t *self, wifi_power_management_t power_management) {
    check_for_enabled(self);

    uint8_t mode;
    switch (power_management) {
        case POWER_MANAGEMENT_MIN:
            mode = 1;
            break;
        case POWER_MANAGEMENT_NONE:
            mode = 0;
            break;
        case POWER_MANAGEMENT_MAX:
            mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q not implemented"), MP_QSTR_POWER_MANAGEMENT_MAX);
            break;
        case POWER_MANAGEMENT_UNKNOWN:
            // This should be prevented in shared-bindings.
            return;
    }

    const uint8_t *params[1] = { &mode };
    size_t param_lengths[1] = { 1 };
    uint8_t result = 0;
    uint8_t *responses[1] = { &result };
    size_t response_lengths[1] = { sizeof(result) };

    size_t num_responses = wifi_radio_send_command_get_response(self, SET_POWER_MODE_CMD,
        params, param_lengths, LENGTHS_8, MP_ARRAY_SIZE(params),
        responses, response_lengths, LENGTHS_8, MP_ARRAY_SIZE(responses),
        AIRLIFT_DEFAULT_TIMEOUT_MS);

    if (num_responses == 0 || result != 1) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Failed"));
    }

    self->power_management = power_management;
}

mp_obj_t common_hal_wifi_radio_get_mac_address_ap(wifi_radio_obj_t *self) {
    mp_raise_NotImplementedError(NULL);
}

void common_hal_wifi_radio_set_mac_address_ap(wifi_radio_obj_t *self, const uint8_t *mac) {
    mp_raise_NotImplementedError(NULL);
}

// start_channel and stop_channel are not implemented by AirLift.
mp_obj_t common_hal_wifi_radio_start_scanning_networks(wifi_radio_obj_t *self, uint8_t start_channel, uint8_t stop_channel) {
    check_for_enabled(self);

    uint8_t result = 0;
    uint8_t *responses[1] = { &result };
    size_t response_lengths[1] = { sizeof(result) };

    size_t num_responses = wifi_radio_send_command_get_response(self, START_SCAN_NETWORKS,
        NULL, NULL, LENGTHS_8, 0,
        responses, response_lengths, LENGTHS_8, MP_ARRAY_SIZE(responses),
        AIRLIFT_DEFAULT_TIMEOUT_MS);

    if (num_responses == 0 || result != 1) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Failed"));
    }

    if (self->current_scan) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Already scanning for wifi networks"));
    }
    if (!common_hal_wifi_radio_get_enabled(self)) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("WiFi is not enabled"));
    }

    return wifi_scannednetworks_do_scan(self);
}

void common_hal_wifi_radio_stop_scanning_networks(wifi_radio_obj_t *self) {
    // Nothing to do: the scan is done all at once.
}

void common_hal_wifi_radio_start_station(wifi_radio_obj_t *self) {
    common_hal_wifi_radio_set_enabled(self, true);
}

void common_hal_wifi_radio_stop_station(wifi_radio_obj_t *self) {
    common_hal_wifi_radio_set_enabled(self, false);
}

void common_hal_wifi_radio_start_ap(wifi_radio_obj_t *self, uint8_t *ssid, size_t ssid_len, uint8_t *password, size_t password_len, uint8_t channel, uint32_t authmode, uint8_t max_connections) {
    mp_raise_NotImplementedError(NULL);
}

bool common_hal_wifi_radio_get_ap_active(wifi_radio_obj_t *self) {
    return mp_const_false;
}

void common_hal_wifi_radio_stop_ap(wifi_radio_obj_t *self) {
    // Doing nothing when not started is the same behavior on other ports.
}

mp_obj_t common_hal_wifi_radio_get_stations_ap(wifi_radio_obj_t *self) {
    return mp_const_none;
}

wifi_radio_error_t common_hal_wifi_radio_connect(wifi_radio_obj_t *self, uint8_t *ssid, size_t ssid_len, uint8_t *password, size_t password_len, uint8_t channel, mp_float_t timeout, uint8_t *bssid, size_t bssid_len) {
    if (bssid_len > 0) {
        mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q not implemented"), MP_QSTR_bssid);
    }

    uint8_t result = 0;
    uint8_t *responses[1] = { &result };
    size_t response_lengths[1] = { sizeof(result) };
    size_t num_responses;

    if (password_len > 0) {
        const uint8_t *params[2] = { ssid, password };
        size_t param_lengths[2] = { ssid_len, password_len };

        num_responses = wifi_radio_send_command_get_response(self, SET_PASSPHRASE_CMD,
            params, param_lengths, LENGTHS_8, MP_ARRAY_SIZE(params),
            responses, response_lengths, LENGTHS_8, MP_ARRAY_SIZE(responses),
            AIRLIFT_DEFAULT_TIMEOUT_MS);
    } else {
        const uint8_t *params[1] = { ssid };
        size_t param_lengths[1] = { ssid_len };

        num_responses = wifi_radio_send_command_get_response(self, SET_NET_CMD,
            params, param_lengths, LENGTHS_8, MP_ARRAY_SIZE(params),
            responses, response_lengths, LENGTHS_8, MP_ARRAY_SIZE(responses),
            AIRLIFT_DEFAULT_TIMEOUT_MS);
    }

    if (num_responses == 0 || result != 1) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Failed"));
    }

    // Wait for connection to be established.
    uint64_t timeout_ms = timeout * 1000;
    if (timeout_ms == 0) {
        timeout_ms = 8000;
    }
    uint64_t end_time = common_hal_time_monotonic_ms() + timeout_ms;
    while (common_hal_time_monotonic_ms() < end_time) {
        RUN_BACKGROUND_TASKS;
        if (mp_hal_is_interrupted()) {
            break;
        }
        mp_hal_delay_ms(200);

        wl_status_t status = wifi_radio_get_connection_status(self);
        switch (status) {
            case WL_NO_SHIELD:
                return WIFI_RADIO_ERROR_NO_RADIO;

            case WL_NO_SSID_AVAIL:
                return WIFI_RADIO_ERROR_NO_AP_FOUND;

            case WL_STOPPED:
            case WL_IDLE_STATUS:
            case WL_SCAN_COMPLETED:
                continue;

            case WL_CONNECTED:
                return WIFI_RADIO_ERROR_NONE;

            case WL_CONNECT_FAILED:
            case WL_CONNECTION_LOST:
            case WL_DISCONNECTED:
                return WIFI_RADIO_ERROR_CONNECTION_FAIL;

            default:
                return WIFI_RADIO_ERROR_UNSPECIFIED;
        }
    }

    // We either timed out or got interrupted.
    return WIFI_RADIO_ERROR_UNSPECIFIED;
}

bool common_hal_wifi_radio_get_connected(wifi_radio_obj_t *self) {
    return wifi_radio_get_connection_status(self) == WL_CONNECTED;
}

mp_obj_t common_hal_wifi_radio_get_ap_info(wifi_radio_obj_t *self) {
    if (!common_hal_wifi_radio_get_enabled(self)) {
        return mp_const_none;
    }

    // Create a Network object and fill it in.
    wifi_network_obj_t *network = mp_obj_malloc(wifi_network_obj_t, &wifi_network_type);

    const uint8_t ignored = 0xFF;
    const uint8_t *params[1] = { &ignored };
    size_t param_lengths[1] = { 1 };

    {
        uint8_t ssid[MAX_SSID_LENGTH] = { 0 };
        uint8_t *responses[1] = { ssid };
        size_t response_lengths[1] = { sizeof(ssid) };

        network->ssid = mp_const_none;
        if (wifi_radio_send_command_get_response(self, GET_CURR_SSID_CMD,
            params, param_lengths, LENGTHS_8, MP_ARRAY_SIZE(params),
            responses, response_lengths, LENGTHS_8, MP_ARRAY_SIZE(responses),
            AIRLIFT_DEFAULT_TIMEOUT_MS) > 0) {
            network->ssid = mp_obj_new_str((const char *)ssid, response_lengths[0]);
        }
    }

    {
        uint8_t bssid[MAC_ADDRESS_LENGTH] = { 0 };
        uint8_t *responses[1] = { bssid };
        size_t response_lengths[1] = { sizeof(bssid) };

        network->ssid = mp_const_none;
        if (wifi_radio_send_command_get_response(self, GET_CURR_BSSID_CMD,
            params, param_lengths, LENGTHS_8, MP_ARRAY_SIZE(params),
            responses, response_lengths, LENGTHS_8, MP_ARRAY_SIZE(responses),
            AIRLIFT_DEFAULT_TIMEOUT_MS) > 0) {
            network->bssid = mp_obj_new_bytes(bssid, MAC_ADDRESS_LENGTH);
        }
    }

    {
        int32_t rssi = 0;
        uint8_t *responses[1] = { (uint8_t *)&rssi };
        size_t response_lengths[1] = { sizeof(rssi) };

        network->rssi = mp_const_none;
        if (wifi_radio_send_command_get_response(self, GET_CURR_RSSI_CMD,
            params, param_lengths, LENGTHS_8, MP_ARRAY_SIZE(params),
            responses, response_lengths, LENGTHS_8, MP_ARRAY_SIZE(responses),
            AIRLIFT_DEFAULT_TIMEOUT_MS) > 0) {
            network->rssi = mp_obj_new_int(rssi);
        }
    }

    // Channel is not available for the current AP we are using.
    network->channel = mp_const_none;

    {
        uint8_t esp_authmode = 0;
        uint8_t *responses[1] = { &esp_authmode };
        size_t response_lengths[1] = { sizeof(esp_authmode) };

        if (wifi_radio_send_command_get_response(self, GET_CURR_ENCT_CMD,
            params, param_lengths, LENGTHS_8, MP_ARRAY_SIZE(params),
            responses, response_lengths, LENGTHS_8, MP_ARRAY_SIZE(responses),
            AIRLIFT_DEFAULT_TIMEOUT_MS) > 0) {

            // Convert AirLift authmode to tuple of wifi.AuthMode objects.
            network->authmode = esp_authmode_to_wifi_authmode_tuple(esp_authmode);
        }
    }

    // country code not available on AirLift.
    network->country = MP_OBJ_NEW_QSTR(MP_QSTR_);

    return network;
}

mp_obj_t common_hal_wifi_radio_get_ipv4_gateway(wifi_radio_obj_t *self) {
    check_for_enabled(self);

    uint8_t gateway[IPV4_LENGTH];
    if (wifi_radio_get_address_info(self, NULL, NULL, gateway)) {
        return common_hal_ipaddress_new_ipv4address_from_bytes(gateway);
    } else {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Failed"));
    }
}

mp_obj_t common_hal_wifi_radio_get_ipv4_gateway_ap(wifi_radio_obj_t *self) {
    return mp_const_none;
}

mp_obj_t common_hal_wifi_radio_get_ipv4_subnet(wifi_radio_obj_t *self) {
    check_for_enabled(self);

    uint8_t mask[IPV4_LENGTH];
    if (wifi_radio_get_address_info(self, NULL, mask, NULL)) {
        return common_hal_ipaddress_new_ipv4address_from_bytes(mask);
    } else {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Failed"));
    }
}

mp_obj_t common_hal_wifi_radio_get_ipv4_subnet_ap(wifi_radio_obj_t *self) {
    return mp_const_none;
}

mp_obj_t common_hal_wifi_radio_get_addresses(wifi_radio_obj_t *self) {
    check_for_enabled(self);

    vstr_t vstr;
    mp_print_t print;
    vstr_init_print(&vstr, 16, &print);
    mp_obj_print_helper(&print, common_hal_wifi_radio_get_ipv4_address(self), PRINT_STR);
    return mp_obj_new_tuple(1, mp_obj_new_str_from_vstr(&vstr));
}

mp_obj_t common_hal_wifi_radio_get_addresses_ap(wifi_radio_obj_t *self) {
    return mp_const_empty_tuple;
}

uint32_t wifi_radio_get_ipv4_address(wifi_radio_obj_t *self) {
    check_for_enabled(self);

    uint8_t ip[IPV4_LENGTH] = { 0 };
    wifi_radio_get_address_info(self, ip, NULL, NULL);
    return ipv4_bytes_to_uint32(ip);
}

mp_obj_t common_hal_wifi_radio_get_ipv4_address(wifi_radio_obj_t *self) {
    check_for_enabled(self);

    uint8_t ip[IPV4_LENGTH];
    if (wifi_radio_get_address_info(self, ip, NULL, NULL)) {
        return common_hal_ipaddress_new_ipv4address_from_bytes(ip);
    } else {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Failed"));
    }
}

mp_obj_t common_hal_wifi_radio_get_ipv4_address_ap(wifi_radio_obj_t *self) {
    return mp_const_none;
}

mp_obj_t common_hal_wifi_radio_get_ipv4_dns(wifi_radio_obj_t *self) {
    check_for_enabled(self);

    mp_obj_t dns[2];
    size_t count;

    wifi_radio_get_dns_config(self, dns, &count);
    if (count == 0) {
        return mp_const_none;
    }

    // Return just the first DNS entry.
    return dns[0];
}

void common_hal_wifi_radio_set_ipv4_dns(wifi_radio_obj_t *self, mp_obj_t ipv4_dns_addr) {
    check_for_enabled(self);

    uint8_t valid_params_arg = 0;

    uint8_t dns_bytes[IPV4_LENGTH];
    uint8_t zero_dns_bytes[IPV4_LENGTH] = {0};
    ipaddress_ipv4address_to_bytes(ipv4_dns_addr, dns_bytes);

    const uint8_t *params[3] = { &valid_params_arg, dns_bytes, zero_dns_bytes };
    size_t param_lengths[3] = { 1, IPV4_LENGTH, IPV4_LENGTH };
    uint8_t *responses[1];
    size_t response_lengths[1];

    size_t num_responses = wifi_radio_send_command_get_response(self, SET_DNS_CONFIG,
        params, param_lengths, LENGTHS_8, MP_ARRAY_SIZE(params),
        responses, response_lengths, LENGTHS_8, MP_ARRAY_SIZE(responses),
        AIRLIFT_DEFAULT_TIMEOUT_MS);

    if (num_responses == 0 || responses[0][0] != 1) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Failed"));
    }
}

void common_hal_wifi_radio_start_dhcp_client(wifi_radio_obj_t *self, bool ipv4, bool ipv6) {
    if (ipv6) {
        mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q"), MP_QSTR_ipv6);
    }
}

void common_hal_wifi_radio_stop_dhcp_client(wifi_radio_obj_t *self) {
    // not controllable
}

void common_hal_wifi_radio_start_dhcp_server(wifi_radio_obj_t *self) {
    mp_raise_NotImplementedError(NULL);
}

void common_hal_wifi_radio_stop_dhcp_server(wifi_radio_obj_t *self) {
    mp_raise_NotImplementedError(NULL);
}

void common_hal_wifi_radio_set_ipv4_address(wifi_radio_obj_t *self, mp_obj_t ipv4, mp_obj_t netmask, mp_obj_t gateway, mp_obj_t ipv4_dns) {
    check_for_enabled(self);

    uint8_t valid_params_arg = 0;

    uint8_t ipv4_bytes[IPV4_LENGTH];
    ipaddress_ipv4address_to_bytes(ipv4, ipv4_bytes);

    uint8_t netmask_bytes[IPV4_LENGTH];
    ipaddress_ipv4address_to_bytes(netmask, netmask_bytes);

    uint8_t gateway_bytes[IPV4_LENGTH];
    ipaddress_ipv4address_to_bytes(gateway, gateway_bytes);

    // Notice order is different.
    const uint8_t *params[4] = { &valid_params_arg, ipv4_bytes, gateway_bytes, netmask_bytes };
    size_t param_lengths[4] = { 1, IPV4_LENGTH, IPV4_LENGTH, IPV4_LENGTH };

    uint8_t result = 0;
    uint8_t *responses[1] = { &result };
    size_t response_lengths[1] = { sizeof(result) };

    size_t num_responses = wifi_radio_send_command_get_response(self, SET_IP_CONFIG,
        params, param_lengths, LENGTHS_8, MP_ARRAY_SIZE(params),
        responses, response_lengths, LENGTHS_8, MP_ARRAY_SIZE(responses),
        AIRLIFT_DEFAULT_TIMEOUT_MS);

    if (num_responses == 0 || result != 1) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Failed"));
    }

    if (ipv4_dns != mp_const_none) {
        common_hal_wifi_radio_set_ipv4_dns(self, ipv4_dns);
    }
}

void common_hal_wifi_radio_set_ipv4_address_ap(wifi_radio_obj_t *self, mp_obj_t ipv4, mp_obj_t netmask, mp_obj_t gateway) {
    mp_raise_NotImplementedError(NULL);
}

mp_int_t common_hal_wifi_radio_ping(wifi_radio_obj_t *self, mp_obj_t ip_address, mp_float_t timeout) {
    check_for_enabled(self);

    uint8_t ip_bytes[IPV4_LENGTH];
    ipaddress_ipv4address_to_bytes(ip_address, ip_bytes);
    const uint8_t ttl = 250;
    const uint8_t *params[2] = { ip_bytes, &ttl };
    size_t param_lengths[2] = { IPV4_LENGTH, 1 };

    uint8_t ping_msecs_bytes[2] = { 0 };
    uint8_t *responses[1] = { ping_msecs_bytes };
    size_t response_lengths[1] = { sizeof(ping_msecs_bytes) };

    size_t num_responses = wifi_radio_send_command_get_response(self, PING_CMD,
        params, param_lengths, LENGTHS_8, MP_ARRAY_SIZE(params),
        responses, response_lengths, LENGTHS_8, MP_ARRAY_SIZE(responses),
        AIRLIFT_DEFAULT_TIMEOUT_MS);

    if (num_responses > 0 && response_lengths[0] >= 2) {
        // Returned as little-endian, unlike data length values, which are big-endian.
        uint16_t ms = le_uint8_bytes_to_uint16(ping_msecs_bytes);
        return ms;
    }

    return -1;
}

void common_hal_wifi_radio_gc_collect(wifi_radio_obj_t *self) {
    gc_collect_ptr(self);
}

mp_obj_t common_hal_wifi_radio_get_dns(wifi_radio_obj_t *self) {
    check_for_enabled(self);

    mp_obj_t dns[2];
    size_t count;

    wifi_radio_get_dns_config(self, dns, &count);

    // Convert the returned IPv4Address objects to strings.
    // Return a tuple of those strings.

    mp_obj_t str[2];
    for (size_t i = 0; i < count; i++) {
        vstr_t vstr;
        mp_print_t print;
        vstr_init_print(&vstr, 16, &print);
        mp_obj_print_helper(&print, dns[i], PRINT_STR);
        str[i] = mp_obj_new_str_from_vstr(&vstr);
    }

    return mp_obj_new_tuple(count, str);
}

// Takes a sequence of strings of IP addresses.
// TODO: It could take host names, but that's not handled by raspberrypi right now.
void common_hal_wifi_radio_set_dns(wifi_radio_obj_t *self, mp_obj_t dns_addrs_obj) {
    check_for_enabled(self);

    mp_int_t len = mp_obj_get_int(mp_obj_len(dns_addrs_obj));
    mp_arg_validate_length_max(len, 2, MP_QSTR_dns);

    uint8_t zero_byte = 0;
    // Two 4-byte sequences.
    uint8_t dns8[2][IPV4_LENGTH] = {0};

    for (mp_int_t i = 0; i < len; i++) {
        uint32_t dns32;
        mp_obj_t dns_addr_obj = mp_obj_subscr(dns_addrs_obj, MP_OBJ_NEW_SMALL_INT(0), MP_OBJ_SENTINEL);
        mp_arg_validate_type_string(dns_addr_obj, MP_QSTR_dns);
        GET_STR_DATA_LEN(dns_addr_obj, str_data, str_len);
        if (!ipaddress_parse_ipv4address((const char *)str_data, str_len, &dns32)) {
            mp_raise_ValueError(MP_ERROR_TEXT("Not a valid IP string"));
        }
        ipv4_uint32_to_bytes(dns32, dns8[i]);
    }

    const uint8_t *params[3] = { &zero_byte, dns8[0], dns8[1] };
    size_t param_lengths[3] = { 1, IPV4_LENGTH, IPV4_LENGTH };

    uint8_t result = 0;
    uint8_t *responses[1] = { &result };
    size_t response_lengths[1] = { sizeof(result) };

    size_t num_responses = wifi_radio_send_command_get_response(self, SET_DNS_CONFIG,
        params, param_lengths, LENGTHS_8, MP_ARRAY_SIZE(params),
        responses, response_lengths, LENGTHS_8, MP_ARRAY_SIZE(responses),
        AIRLIFT_DEFAULT_TIMEOUT_MS);

    if (num_responses == 0 || result != 1) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Failed"));
    }
}
