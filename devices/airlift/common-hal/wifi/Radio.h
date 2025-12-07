// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/wifi/ScannedNetworks.h"

// Forward declarations due to mutually referenced typedef struct definitions.
typedef struct wifi_scannednetworks_obj_t wifi_scannednetworks_obj_t;

// Connection modes
typedef enum {
    AIRLIFT_TCP_MODE = 0,
    AIRLIFT_UDP_MODE = 1,
    AIRLIFT_TLS_MODE = 2,
} airlift_conn_mode_t;

typedef struct wifi_radio_obj_t {
    mp_obj_base_t base;
    wifi_scannednetworks_obj_t *current_scan;
    uint32_t ping_elapsed_time;
    bool enabled;
    bool ap_mode;
    bool sta_mode;
    uint8_t retries_left;
    uint8_t starting_retries;
    uint8_t last_disconnect_reason;
    char *hostname;
    wifi_power_management_t power_management;

    busio_spi_obj_t *spi;
    digitalio_digitalinout_obj_t *cs_dio;
    digitalio_digitalinout_obj_t *ready_dio;
    digitalio_digitalinout_obj_t *reset_dio;
    digitalio_digitalinout_obj_t *gpio0_dio;
    int8_t tls_socket;  // -1 if no TLS socket allocated
} wifi_radio_obj_t;

extern void common_hal_wifi_radio_gc_collect(wifi_radio_obj_t *self);

// ESP32 SPI protocol command constants
#define START_CMD 0xE0
#define END_CMD 0xEE
#define ERR_CMD 0xEF
#define REPLY_FLAG 0x80
#define CMD_FLAG 0

// Command opcodes
#define SET_NET_CMD 0x10
#define SET_PASSPHRASE_CMD 0x11
#define SET_KEY_CMD 0x12
// #define TEST_CMD 0x13 // not implemented
#define SET_IP_CONFIG 0x14
#define SET_DNS_CONFIG 0x15
#define SET_HOSTNAME 0x16
#define SET_POWER_MODE_CMD 0x17
#define SET_AP_NET_CMD 0x18
#define SET_AP_PASSPHRASE_CMD 0x19
#define SET_DEBUG_CMD 0x1A
#define GET_TEMPERATURE_CMD 0x1B
#define GET_DNS_CONFIG_CMD 0x1E
#define GET_REASON_CODE_CMD 0x1F

#define GET_CONN_STATUS_CMD 0x20
#define GET_IPADDR_CMD 0x21
#define GET_MACADDR_CMD 0x22
#define GET_CURR_SSID_CMD 0x23
#define GET_CURR_BSSID_CMD 0x24
#define GET_CURR_RSSI_CMD 0x25
#define GET_CURR_ENCT_CMD 0x26
#define SCAN_NETWORKS 0x27
#define START_SERVER_TCP_CMD 0x28
#define GET_STATE_TCP_CMD 0x29
#define DATA_SENT_TCP_CMD 0x2A
#define AVAIL_DATA_TCP_CMD 0x2B
#define GET_DATA_TCP_CMD 0x2C
#define START_CLIENT_TCP_CMD 0x2D
#define STOP_CLIENT_TCP_CMD 0x2E
#define GET_CLIENT_STATE_TCP_CMD 0x2F

#define DISCONNECT_CMD 0x30
// GET_IDX_SSID_CMD 0x31 // not implemented
#define GET_IDX_RSSI_CMD 0x32
#define GET_IDX_ENCT_CMD 0x33
#define REQ_HOST_BY_NAME_CMD 0x34
#define GET_HOST_BY_NAME_CMD 0x35
#define START_SCAN_NETWORKS 0x36
#define GET_FW_VERSION_CMD 0x37
// GET_TEST_CMD 0x38 // not implemented
#define SEND_UDP_DATA_CMD 0x39
#define GET_REMOTE_DATA_CMD 0x3A
#define GET_TIME 0x3B
#define GET_IDX_BSSID_CMD 0x3C
#define GET_IDX_CHAN_CMD 0x3D
#define PING_CMD 0x3E
#define GET_SOCKET_CMD 0x3F

#define SET_CLI_CERT 0x40
// Adafruit
#define SET_PK 0x41
#define SEND_DATA_TCP_CMD 0x44
#define GET_DATABUF_TCP_CMD 0x45
#define INSERT_DATABUF_TCP_CMD 0x46
// Adafruit
#define SET_ENT_IDENT_CMD 0x4A
#define SET_ENT_UNAME_CMD 0x4B
#define SET_ENT_PASSWD_CMD 0x4C
#define SET_ENT_ENABLE_CMD 0x4F

#define SET_PIN_MODE_CMD 0x50
#define SET_DIGITAL_WRITE_CMD 0x51
#define SET_ANALOG_WRITE_CMD 0x52
#define SET_DIGITAL_READ_CMD 0x53
#define SET_ANALOG_READ_CMD 0x54

// Not implemented in Adafruit NINA-FW
#define WRITE_FILE 0x60
#define READ_FILE 0x61
#define DELETE_FILE 0x62
#define EXISTS_FILE 0x63
#define DOWNLOAD_FILE 0x64
#define APPLY_OTA_COMMAND 0x65
#define RENAME_FILE 0x66
#define DOWNLOAD_OTA 0x67

// Low-level socket operations.
#define SOCKET_SOCKET_CMD 0x70
#define SOCKET_CLOSE_CMD 0x71
#define SOCKET_ERRNO_CMD 0x72
#define SOCKET_BIND_CMD 0x73
#define SOCKET_LISTEN_CMD 0x74
#define SOCKET_ACCEPT_CMD 0x75
#define SOCKET_CONNECT_CMD 0x76
#define SOCKET_SEND_CMD 0x77
#define SOCKET_RECV_CMD 0x78
#define SOCKET_SENDTO_CMD 0x79
#define SOCKET_RECVFROM_CMD 0x7A
#define SOCKET_IOCTL_CMD 0x7B
#define SOCKET_POLL_CMD 0x7C
#define SOCKET_SETSOCKOPT_CMD 0x7D
#define SOCKET_GETSOCKOPT_CMD 0x7E
#define SOCKET_GETPEERNAME_CMD 0x7F

typedef enum {
    WL_NO_SHIELD = 255, // for compatibility with WiFi Shield library
    WL_STOPPED = 254,
    WL_IDLE_STATUS = 0,
    WL_NO_SSID_AVAIL = 1,
    WL_SCAN_COMPLETED = 2,
    WL_CONNECTED = 3,
    WL_CONNECT_FAILED = 4,
    WL_CONNECTION_LOST = 5,
    WL_DISCONNECTED = 6
} wl_status_t;

#define AIRLIFT_DEFAULT_TIMEOUT_MS (1000)

// Maximum is 10 in NINA-FW.
#define AIRLIFT_MAX_NETWORKS (10)
// SSID does not include a terminating null byte, because the length is passed.
#define MAX_SSID_LENGTH (32)
// NINA-FW allows this length internally. Does not include trailing null.
#define MAX_HOSTNAME_LENGTH (255)
#define IPV4_LENGTH (4)
#define AIRLIFT_MAX_FIRMWARE_VERSION_LENGTH (6)

// Param and response lengths can be 8-bit, or 16-bit big-endian.
// Most are 8-bit, but potentially large data buffers have 16-bit lengths.
typedef enum {
    LENGTHS_8 = 8,
    LENGTHS_16 = 16,
} lengths_size_t;

// Communication with AirLift.
bool wifi_radio_send_command(wifi_radio_obj_t *self, uint8_t cmd,
    const uint8_t **params, const size_t *param_lengths, lengths_size_t param_lengths_size, size_t num_params);
size_t wifi_radio_wait_response_cmd(wifi_radio_obj_t *self, uint8_t cmd,
    uint8_t **responses, size_t *response_lengths, lengths_size_t response_lengths_size, size_t max_responses,
    uint32_t response_timeout_ms);
size_t wifi_radio_send_command_get_response(wifi_radio_obj_t *self, uint8_t cmd,
    const uint8_t **params, const size_t *param_lengths, lengths_size_t param_lengths_size_t, size_t num_params,
    uint8_t **responses, size_t *response_lengths, lengths_size_t response_lengths_size, size_t max_responses,
    uint32_t response_timeout_ms);
wl_status_t wifi_radio_get_connection_status(wifi_radio_obj_t *self);

inline void be_uint16_to_uint8_bytes(uint16_t value, uint8_t *bytes) {
    bytes[0] = value >> 8;
    bytes[1] = value & 0xff;
}

inline uint16_t be_uint8_bytes_to_uint16(uint8_t *bytes) {
    return bytes[1] | (bytes[0] << 8);
}

inline void le_uint16_to_uint8_bytes(uint16_t value, uint8_t *bytes) {
    bytes[0] = value & 0xff;
    bytes[1] = value >> 8;
}

inline uint16_t le_uint8_bytes_to_uint16(uint8_t *bytes) {
    return bytes[0] | (bytes[1] << 8);
}

inline uint32_t le_uint8_bytes_to_uint32(uint8_t *bytes) {
    return bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24);
}
