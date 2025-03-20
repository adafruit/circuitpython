// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "common-hal/wifi/__init__.h"
#include "shared-bindings/wifi/__init__.h"

#include "shared-bindings/ipaddress/IPv4Address.h"
#include "shared-bindings/wifi/Monitor.h"
#include "shared-bindings/wifi/Radio.h"
#include "bindings/zephyr_kernel/__init__.h"
#include "common-hal/socketpool/__init__.h"

#include "py/gc.h"
#include "py/mpstate.h"
#include "py/runtime.h"

wifi_radio_obj_t common_hal_wifi_radio_obj;

#include "supervisor/port.h"
#include "supervisor/workflow.h"

#if CIRCUITPY_STATUS_BAR
#include "supervisor/shared/status_bar.h"
#endif

#include <zephyr/kernel.h>
#include <zephyr/net/wifi_mgmt.h>

#define MAC_ADDRESS_LENGTH 6

static void schedule_background_on_cp_core(void *arg) {
    #if CIRCUITPY_STATUS_BAR
    supervisor_status_bar_request_update(false);
    #endif

    // CircuitPython's VM is run in a separate FreeRTOS task from wifi callbacks. So, we have to
    // notify the main task every time in case it's waiting for us.
    port_wake_main_task();
}

static struct net_mgmt_event_callback wifi_cb;
static struct net_mgmt_event_callback ipv4_cb;

static void _event_handler(struct net_mgmt_event_callback *cb, uint32_t mgmt_event, struct net_if *iface) {
    wifi_radio_obj_t *self = &common_hal_wifi_radio_obj;
    printk("_event_handler cb %p event %08x if %p\n", cb, mgmt_event, iface);

    switch (mgmt_event) {
        case NET_EVENT_WIFI_SCAN_RESULT:
            printk("NET_EVENT_WIFI_SCAN_RESULT\n");
            struct wifi_scan_result *result = (struct wifi_scan_result *)cb->info;
            if (self->current_scan != NULL) {
                wifi_scannednetworks_scan_result(self->current_scan, result);
            }
            break;
        case NET_EVENT_WIFI_SCAN_DONE:
            printk("NET_EVENT_WIFI_SCAN_DONE\n");
            if (self->current_scan != NULL) {
                k_poll_signal_raise(&self->current_scan->channel_done, 0);
            }
            break;
        case NET_EVENT_WIFI_CONNECT_RESULT:
            printk("NET_EVENT_WIFI_CONNECT_RESULT\n");
            break;
        case NET_EVENT_WIFI_DISCONNECT_RESULT:
            printk("NET_EVENT_WIFI_DISCONNECT_RESULT\n");
            break;
        case NET_EVENT_WIFI_IFACE_STATUS:
            printk("NET_EVENT_WIFI_IFACE_STATUS\n");
            break;
        case NET_EVENT_WIFI_TWT:
            printk("NET_EVENT_WIFI_TWT\n");
            break;
        case NET_EVENT_WIFI_TWT_SLEEP_STATE:
            printk("NET_EVENT_WIFI_TWT_SLEEP_STATE\n");
            break;
        case NET_EVENT_WIFI_RAW_SCAN_RESULT:
            printk("NET_EVENT_WIFI_RAW_SCAN_RESULT\n");
            break;
        case NET_EVENT_WIFI_DISCONNECT_COMPLETE:
            printk("NET_EVENT_WIFI_DISCONNECT_COMPLETE\n");
            break;
        case NET_EVENT_WIFI_SIGNAL_CHANGE:
            printk("NET_EVENT_WIFI_SIGNAL_CHANGE\n");
            break;
        case NET_EVENT_WIFI_NEIGHBOR_REP_COMP:
            printk("NET_EVENT_WIFI_NEIGHBOR_REP_COMP\n");
            break;
        case NET_EVENT_WIFI_AP_ENABLE_RESULT:
            printk("NET_EVENT_WIFI_AP_ENABLE_RESULT\n");
            break;
        case NET_EVENT_WIFI_AP_DISABLE_RESULT:
            printk("NET_EVENT_WIFI_AP_DISABLE_RESULT\n");
            break;
        case NET_EVENT_WIFI_AP_STA_CONNECTED:
            printk("NET_EVENT_WIFI_AP_STA_CONNECTED\n");
            break;
        case NET_EVENT_WIFI_AP_STA_DISCONNECTED:
            printk("NET_EVENT_WIFI_AP_STA_DISCONNECTED\n");
            break;
    }
}

// static void event_handler(void *arg, esp_event_base_t event_base,
//     int32_t event_id, void *event_data) {
// // This runs on the PRO CORE! It cannot share CP interrupt enable/disable
// // directly.
// wifi_radio_obj_t *radio = arg;
// if (event_base == WIFI_EVENT) {
//     switch (event_id) {
//         case WIFI_EVENT_SCAN_DONE:
//             ESP_LOGW(TAG, "scan");
//             xEventGroupSetBits(radio->event_group_handle, WIFI_SCAN_DONE_BIT);
//             break;
//         case WIFI_EVENT_AP_START:
//             ESP_LOGW(TAG, "ap start");
//             break;
//         case WIFI_EVENT_AP_STOP:
//             ESP_LOGW(TAG, "ap stop");
//             break;
//         case WIFI_EVENT_AP_STACONNECTED:
//             break;
//         case WIFI_EVENT_AP_STADISCONNECTED:
//             break;
//         case WIFI_EVENT_STA_START:
//             ESP_LOGW(TAG, "sta start");
//             break;
//         case WIFI_EVENT_STA_STOP:
//             ESP_LOGW(TAG, "sta stop");
//             break;
//         case WIFI_EVENT_STA_CONNECTED:
//             ESP_LOGW(TAG, "connected");
//             break;
//         case WIFI_EVENT_STA_DISCONNECTED: {
//             ESP_LOGW(TAG, "disconnected");
//             wifi_event_sta_disconnected_t *d = (wifi_event_sta_disconnected_t *)event_data;
//             uint8_t reason = d->reason;
//             ESP_LOGW(TAG, "reason %d 0x%02x", reason, reason);
//             if (radio->retries_left > 0 &&
//                 reason != WIFI_REASON_AUTH_FAIL &&
//                 reason != WIFI_REASON_NO_AP_FOUND &&
//                 reason != WIFI_REASON_ASSOC_LEAVE) {
//                 radio->retries_left--;
//                 ESP_LOGI(TAG, "Retrying connect. %d retries remaining", radio->retries_left);
//                 esp_wifi_connect();
//                 return;
//             }

//             radio->last_disconnect_reason = reason;
//             xEventGroupSetBits(radio->event_group_handle, WIFI_DISCONNECTED_BIT);
//             break;
//         }

//         // Cases to handle later.
//         // case WIFI_EVENT_STA_AUTHMODE_CHANGE:
//         default: {
//             ESP_LOGW(TAG, "event %ld 0x%02ld", event_id, event_id);
//             break;
//         }
//     }
// }

// if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
//     ESP_LOGW(TAG, "got ip");
//     radio->retries_left = radio->starting_retries;
//     xEventGroupSetBits(radio->event_group_handle, WIFI_CONNECTED_BIT);
// }
// // Use IPC to ensure we run schedule background on the same core as CircuitPython.
// #if defined(CONFIG_FREERTOS_UNICORE) && CONFIG_FREERTOS_UNICORE
// schedule_background_on_cp_core(NULL);
// #else
// // This only blocks until the start of the function. That's ok since the PRO
// // core shouldn't care what we do.
// esp_ipc_call(CONFIG_ESP_MAIN_TASK_AFFINITY, schedule_background_on_cp_core, NULL);
// #endif
// }

static bool wifi_inited;
static bool wifi_ever_inited;
static bool wifi_user_initiated;

void common_hal_wifi_init(bool user_initiated) {
    wifi_radio_obj_t *self = &common_hal_wifi_radio_obj;
    printk("common_hal_wifi_init\n");

    if (wifi_inited) {
        if (user_initiated && !wifi_user_initiated) {
            common_hal_wifi_radio_set_enabled(self, true);
        }
        return;
    }
    wifi_inited = true;
    wifi_user_initiated = user_initiated;
    self->base.type = &wifi_radio_type;

    // struct net_if *default_iface = net_if_get_default();
    // printk("default interface %p\n", default_iface);
    // printk("listing network interfaces\n");
    // for (int i = 0; i < 10; i++) {
    //     struct net_if* iface = net_if_get_by_index(i);
    //     if (iface == NULL) {
    //         printk("iface %d is NULL\n", i);
    //         continue;
    //     }
    //     char name[32];
    //     net_if_get_name(iface, name, 32);
    //     printk("iface %d %s\n", i, name);
    // }
    self->sta_netif = net_if_get_wifi_sta();
    self->ap_netif = net_if_get_wifi_sap();
    printk("sta_netif %p\n", self->sta_netif);
    printk("ap_netif %p\n", self->ap_netif);


    struct wifi_iface_status status = { 0 };
    if (self->sta_netif != NULL) {
        CHECK_ZEPHYR_RESULT(net_mgmt(NET_REQUEST_WIFI_IFACE_STATUS, self->sta_netif, &status,
            sizeof(struct wifi_iface_status)));
        if (net_if_is_up(self->sta_netif)) {
            printk("STA is up\n");
        } else {
            printk("STA is down\n");
        }
        if (net_if_is_carrier_ok(self->sta_netif)) {
            printk("STA carrier is ok\n");
        } else {
            printk("STA carrier is not ok\n");
        }
        if (net_if_is_dormant(self->sta_netif)) {
            printk("STA is dormant\n");
        } else {
            printk("STA is not dormant\n");
        }
    }
    if (self->ap_netif != NULL) {
        int res = net_mgmt(NET_REQUEST_WIFI_IFACE_STATUS, self->ap_netif, &status,
            sizeof(struct wifi_iface_status));
        printk("AP status request response %d\n", res);
        if (net_if_is_up(self->ap_netif)) {
            printk("AP is up\n");
        } else {
            printk("AP is down\n");
        }
        if (net_if_is_carrier_ok(self->ap_netif)) {
            printk("AP carrier is ok\n");
        } else {
            printk("AP carrier is not ok\n");
        }
        if (net_if_is_dormant(self->ap_netif)) {
            printk("AP is dormant\n");
        } else {
            printk("AP is not dormant\n");
        }
    }

    // self->started = false;

    // // Even though we just called esp_netif_create_default_wifi_sta,
    // //   station mode isn't actually ready for use until esp_wifi_set_mode()
    // //   is called and the configuration is loaded via esp_wifi_set_config().
    // // Set both convenience flags to false so it's not forgotten.
    // self->sta_mode = 0;
    // self->ap_mode = 0;

    net_mgmt_init_event_callback(&wifi_cb, _event_handler,
        NET_EVENT_WIFI_SCAN_DONE |
        NET_EVENT_WIFI_CONNECT_RESULT |
        NET_EVENT_WIFI_DISCONNECT_RESULT |
        NET_EVENT_WIFI_TWT |
        NET_EVENT_WIFI_RAW_SCAN_RESULT |
        NET_EVENT_WIFI_AP_ENABLE_RESULT |
        NET_EVENT_WIFI_AP_DISABLE_RESULT |
        NET_EVENT_WIFI_AP_STA_CONNECTED |
        NET_EVENT_WIFI_AP_STA_DISCONNECTED);

    net_mgmt_init_event_callback(&ipv4_cb, _event_handler, NET_EVENT_IPV4_ADDR_ADD);

    net_mgmt_add_event_callback(&wifi_cb);
    net_mgmt_add_event_callback(&ipv4_cb);

    // Set the default hostname capped at NET_HOSTNAME_MAX_LEN characters. We trim off
    // the start of the board name (likely manufacturer) because the end is
    // often more unique to the board.
    size_t board_len = MIN(NET_HOSTNAME_MAX_LEN - ((MAC_ADDRESS_LENGTH * 2) + 6), strlen(CIRCUITPY_BOARD_ID));
    size_t board_trim = strlen(CIRCUITPY_BOARD_ID) - board_len;
    // Avoid double _ in the hostname.
    if (CIRCUITPY_BOARD_ID[board_trim] == '_') {
        board_trim++;
    }

    char cpy_default_hostname[board_len + (MAC_ADDRESS_LENGTH * 2) + 6];
    struct net_linkaddr *mac = net_if_get_link_addr(self->sta_netif);
    if (mac->len < MAC_ADDRESS_LENGTH) {
        printk("MAC address too short");
    }
    snprintf(cpy_default_hostname, sizeof(cpy_default_hostname), "cpy-%s-%02x%02x%02x%02x%02x%02x", CIRCUITPY_BOARD_ID + board_trim, mac->addr[0], mac->addr[1], mac->addr[2], mac->addr[3], mac->addr[4], mac->addr[5]);

    if (net_hostname_set(cpy_default_hostname, strlen(cpy_default_hostname)) != 0) {
        printk("setting hostname failed\n");
    }
    // set station mode to avoid the default SoftAP
    common_hal_wifi_radio_start_station(self);
    // start wifi
    common_hal_wifi_radio_set_enabled(self, true);

    printk("common_hal_wifi_init done\n");
}

void wifi_user_reset(void) {
    if (wifi_user_initiated) {
        wifi_reset();
        wifi_user_initiated = false;
    }
}

void wifi_reset(void) {
    printk("wifi_reset\n");
    if (!wifi_inited) {
        return;
    }
    common_hal_wifi_monitor_deinit(MP_STATE_VM(wifi_monitor_singleton));
    wifi_radio_obj_t *radio = &common_hal_wifi_radio_obj;
    common_hal_wifi_radio_set_enabled(radio, false);
    // #ifndef CONFIG_IDF_TARGET_ESP32
    // ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT,
    //     ESP_EVENT_ANY_ID,
    //     radio->handler_instance_all_wifi));
    // ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT,
    //     IP_EVENT_STA_GOT_IP,
    //     radio->handler_instance_got_ip));
    // ESP_ERROR_CHECK(esp_wifi_deinit());
    // esp_netif_destroy(radio->netif);
    // radio->netif = NULL;
    // esp_netif_destroy(radio->ap_netif);
    // radio->ap_netif = NULL;
    // wifi_inited = false;
    // #endif
    supervisor_workflow_request_background();
}

// void ipaddress_ipaddress_to_esp_idf(mp_obj_t ip_address, ip_addr_t *esp_ip_address) {
// if (mp_obj_is_type(ip_address, &ipaddress_ipv4address_type)) {
//     ipaddress_ipaddress_to_esp_idf_ip4(ip_address, (esp_ip4_addr_t *)esp_ip_address);
//     #if LWIP_IPV6
//     esp_ip_address->type = IPADDR_TYPE_V4;
//     #endif
// } else {
//     struct sockaddr_storage addr_storage;
//     socketpool_resolve_host_or_throw(AF_UNSPEC, SOCK_STREAM, mp_obj_str_get_str(ip_address), &addr_storage, 1);
//     sockaddr_to_espaddr(&addr_storage, (esp_ip_addr_t *)esp_ip_address);
// }
// }

// void ipaddress_ipaddress_to_esp_idf_ip4(mp_obj_t ip_address, esp_ip4_addr_t *esp_ip_address) {
// if (!mp_obj_is_type(ip_address, &ipaddress_ipv4address_type)) {
//     mp_raise_ValueError(MP_ERROR_TEXT("Only IPv4 addresses supported"));
// }
// mp_obj_t packed = common_hal_ipaddress_ipv4address_get_packed(ip_address);
// size_t len;
// const char *bytes = mp_obj_str_get_data(packed, &len);
// esp_netif_set_ip4_addr(esp_ip_address, bytes[0], bytes[1], bytes[2], bytes[3]);
// }

void common_hal_wifi_gc_collect(void) {
    common_hal_wifi_radio_gc_collect(&common_hal_wifi_radio_obj);
}

// static mp_obj_t espaddrx_to_str(const void *espaddr, uint8_t esptype) {
//     char buf[IPADDR_STRLEN_MAX];
//     inet_ntop(esptype == ESP_IPADDR_TYPE_V6 ? AF_INET6 : AF_INET, espaddr, buf, sizeof(buf));
//     return mp_obj_new_str(buf, strlen(buf));
// }

// mp_obj_t espaddr_to_str(const esp_ip_addr_t *espaddr) {
//     return espaddrx_to_str(espaddr, espaddr->type);
// }

// mp_obj_t espaddr4_to_str(const esp_ip4_addr_t *espaddr) {
//     return espaddrx_to_str(espaddr, ESP_IPADDR_TYPE_V4);
// }

// mp_obj_t espaddr6_to_str(const esp_ip6_addr_t *espaddr) {
//     return espaddrx_to_str(espaddr, ESP_IPADDR_TYPE_V6);
// }

// mp_obj_t sockaddr_to_str(const struct sockaddr_storage *sockaddr) {
// char buf[IPADDR_STRLEN_MAX];
// #if CIRCUITPY_SOCKETPOOL_IPV6
// if (sockaddr->ss_family == AF_INET6) {
//     const struct sockaddr_in6 *addr6 = (const void *)sockaddr;
//     inet_ntop(AF_INET6, &addr6->sin6_addr, buf, sizeof(buf));
// } else
// #endif
// {
//     const struct sockaddr_in *addr = (const void *)sockaddr;
//     inet_ntop(AF_INET, &addr->sin_addr, buf, sizeof(buf));
// }
// return mp_obj_new_str(buf, strlen(buf));
// }

// mp_obj_t sockaddr_to_tuple(const struct sockaddr_storage *sockaddr) {
// mp_obj_t args[4] = {
//     sockaddr_to_str(sockaddr),
// };
// int n = 2;
// #if CIRCUITPY_SOCKETPOOL_IPV6
// if (sockaddr->ss_family == AF_INET6) {
//     const struct sockaddr_in6 *addr6 = (const void *)sockaddr;
//     args[1] = MP_OBJ_NEW_SMALL_INT(htons(addr6->sin6_port));
//     args[2] = MP_OBJ_NEW_SMALL_INT(addr6->sin6_flowinfo);
//     args[3] = MP_OBJ_NEW_SMALL_INT(addr6->sin6_scope_id);
//     n = 4;
// } else
// #endif
// {
//     const struct sockaddr_in *addr = (const void *)sockaddr;
//     args[1] = MP_OBJ_NEW_SMALL_INT(htons(addr->sin_port));
// }
// return mp_obj_new_tuple(n, args);
// }

// void sockaddr_to_espaddr(const struct sockaddr_storage *sockaddr, esp_ip_addr_t *espaddr) {
// #if CIRCUITPY_SOCKETPOOL_IPV6
// MP_STATIC_ASSERT(IPADDR_TYPE_V4 == ESP_IPADDR_TYPE_V4);
// MP_STATIC_ASSERT(IPADDR_TYPE_V6 == ESP_IPADDR_TYPE_V6);
// MP_STATIC_ASSERT(sizeof(ip_addr_t) == sizeof(esp_ip_addr_t));
// MP_STATIC_ASSERT(offsetof(ip_addr_t, u_addr) == offsetof(esp_ip_addr_t, u_addr));
// MP_STATIC_ASSERT(offsetof(ip_addr_t, type) == offsetof(esp_ip_addr_t, type));
// if (sockaddr->ss_family == AF_INET6) {
//     const struct sockaddr_in6 *addr6 = (const void *)sockaddr;
//     MP_STATIC_ASSERT(sizeof(espaddr->u_addr.ip6.addr) == sizeof(addr6->sin6_addr));
//     memcpy(&espaddr->u_addr.ip6.addr, &addr6->sin6_addr, sizeof(espaddr->u_addr.ip6.addr));
//     espaddr->u_addr.ip6.zone = addr6->sin6_scope_id;
//     espaddr->type = ESP_IPADDR_TYPE_V6;
// } else
// #endif
// {
//     const struct sockaddr_in *addr = (const void *)sockaddr;
//     MP_STATIC_ASSERT(sizeof(espaddr->u_addr.ip4.addr) == sizeof(addr->sin_addr));
//     memcpy(&espaddr->u_addr.ip4.addr, &addr->sin_addr, sizeof(espaddr->u_addr.ip4.addr));
//     espaddr->type = ESP_IPADDR_TYPE_V4;
// }
// }

// void espaddr_to_sockaddr(const esp_ip_addr_t *espaddr, struct sockaddr_storage *sockaddr, int port) {
// #if CIRCUITPY_SOCKETPOOL_IPV6
// if (espaddr->type == ESP_IPADDR_TYPE_V6) {
//     struct sockaddr_in6 *addr6 = (void *)sockaddr;
//     memcpy(&addr6->sin6_addr, &espaddr->u_addr.ip6.addr, sizeof(espaddr->u_addr.ip6.addr));
//     addr6->sin6_scope_id = espaddr->u_addr.ip6.zone;
// } else
// #endif
// {
//     struct sockaddr_in *addr = (void *)sockaddr;
//     memcpy(&addr->sin_addr, &espaddr->u_addr.ip4.addr, sizeof(espaddr->u_addr.ip4.addr));
// }
// }
