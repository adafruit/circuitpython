// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "common-hal/mii/Ethernet.h"
#include "common-hal/mii/__init__.h"
#include "shared-bindings/mii/Ethernet.h"
#include "shared-bindings/mii/__init__.h"

#include <string.h>
#include <errno.h>

#include "bindings/espidf/__init__.h"

#include "supervisor/port.h"
#include "supervisor/workflow.h"
#include "shared/runtime/interrupt_char.h"

#if CIRCUITPY_STATUS_BAR
#include "supervisor/shared/status_bar.h"
#endif

#include "esp_timer.h"

#include "py/runtime.h"
#include "shared-bindings/ipaddress/IPv4Address.h"
#include "shared-module/ipaddress/__init__.h"

#include "esp_eth.h"
#include "esp_eth_mac_esp.h"
#include "esp_eth_phy.h"
#include "esp_eth_phy_802_3.h"
#include "esp_eth_netif_glue.h"
#include "esp_event.h"
#include "esp_mac.h"
#include "esp_netif.h"

#include "lwip/inet.h"
#include "lwip/sockets.h"

#define MAC_ADDRESS_LENGTH 6

static void _ipv4address_to_esp_idf(mp_obj_t ip_address, esp_ip4_addr_t *esp_addr) {
    mp_obj_t packed = common_hal_ipaddress_ipv4address_get_packed(ip_address);
    size_t len;
    const char *bytes = mp_obj_str_get_data(packed, &len);
    esp_netif_set_ip4_addr(esp_addr, bytes[0], bytes[1], bytes[2], bytes[3]);
}

static mp_obj_t _ip4_to_str(const esp_ip4_addr_t *addr) {
    char buf[IPADDR_STRLEN_MAX];
    inet_ntop(AF_INET, addr, buf, sizeof(buf));
    return mp_obj_new_str(buf, strlen(buf));
}

static mp_obj_t _ipaddr_to_str(const esp_ip_addr_t *addr) {
    char buf[IPADDR_STRLEN_MAX];
    inet_ntop(addr->type == ESP_IPADDR_TYPE_V6 ? AF_INET6 : AF_INET, addr, buf, sizeof(buf));
    return mp_obj_new_str(buf, strlen(buf));
}

static void _eth_event_handler(void *arg, esp_event_base_t event_base,
    int32_t event_id, void *event_data) {
    mii_ethernet_obj_t *self = (mii_ethernet_obj_t *)arg;

    if (event_base == ETH_EVENT) {
        switch (event_id) {
            case ETHERNET_EVENT_CONNECTED:
                self->connected = true;
                break;
            case ETHERNET_EVENT_DISCONNECTED:
                self->connected = false;
                break;
            default:
                break;
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_ETH_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        self->ip_info = event->ip_info;
    }

    #if CIRCUITPY_STATUS_BAR
    supervisor_status_bar_request_update(false);
    #endif
    port_wake_main_task();
}

void common_hal_mii_ethernet_construct(mii_ethernet_obj_t *self,
    mii_rmii_obj_t *rmii, uint32_t device) {
    self->base.type = &mii_ethernet_type;
    self->started = false;
    self->connected = false;
    self->eth_handle = NULL;
    self->netif = NULL;
    self->glue = NULL;
    if (rmii == NULL) {
        return;
    }
    if (rmii->mac != NULL) {
        // The EMAC instance behind this bus is already bound to an interface.
        // Raise before creating a second MAC: a failed mac_new would disable
        // the bus clock on error and wound the live interface.
        mp_raise_RuntimeError(MP_ERROR_TEXT("Peripheral in use"));
    }

    // Initialize TCP/IP stack and event loop (may already be done by wifi).
    esp_netif_init();
    esp_event_loop_create_default();

    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    eth_esp32_emac_config_t esp32_emac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG();
    esp32_emac_config.smi_gpio.mdc_num = rmii->mdc;
    esp32_emac_config.smi_gpio.mdio_num = rmii->mdio;
    #if (defined(SOC_EMAC_USE_MULTI_IO_MUX) && SOC_EMAC_USE_MULTI_IO_MUX) || (defined(SOC_EMAC_MII_USE_GPIO_MATRIX) && SOC_EMAC_MII_USE_GPIO_MATRIX)
    esp32_emac_config.emac_dataif_gpio.rmii.crs_dv_num = rmii->rx_dv;
    esp32_emac_config.emac_dataif_gpio.rmii.rxd0_num = rmii->rxd0;
    esp32_emac_config.emac_dataif_gpio.rmii.rxd1_num = rmii->rxd1;
    esp32_emac_config.emac_dataif_gpio.rmii.txd0_num = rmii->txd0;
    esp32_emac_config.emac_dataif_gpio.rmii.txd1_num = rmii->txd1;
    esp32_emac_config.emac_dataif_gpio.rmii.tx_en_num = rmii->tx_en;
    esp32_emac_config.clock_config.rmii.clock_gpio = rmii->refclk;
    #endif

    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&esp32_emac_config, &mac_config);
    if (mac == NULL) {
        // The EMAC instance is already bound to a live interface.
        mp_raise_RuntimeError(MP_ERROR_TEXT("Peripheral in use"));
    }
    // Record the binding so later construct attempts on this bus refuse
    // cleanly before they can wound the live interface.
    rmii->mac = mac;

    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    phy_config.phy_addr = device;
    phy_config.reset_gpio_num = rmii->reset;

    esp_eth_phy_t *phy = esp_eth_phy_new_generic(&phy_config);

    // Probe the PHY's ID registers through the MAC's MDIO master so that a
    // wrong device number fails here instead of silently never linking.
    uint32_t id1 = 0, id2 = 0;
    esp_err_t r1 = mac->read_phy_reg(mac, device, 2, &id1);
    esp_err_t r2 = mac->read_phy_reg(mac, device, 3, &id2);
    if (r1 != ESP_OK || r2 != ESP_OK || id1 == 0xffff || (id1 == 0 && id2 == 0)) {
        mac->del(mac);
        mp_raise_OSError_msg_varg(MP_ERROR_TEXT("%q failure: %d"), MP_QSTR_device, ENODEV);
    }

    // Install the ethernet driver.
    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy);
    CHECK_ESP_RESULT(esp_eth_driver_install(&config, &self->eth_handle));

    // Seed the MAC from eFuse. esp_eth_mac_new_esp32 leaves it all-zero.
    uint8_t mac_addr[MAC_ADDRESS_LENGTH];
    esp_efuse_mac_get_default(mac_addr);
    esp_eth_ioctl(self->eth_handle, ETH_CMD_S_MAC_ADDR, mac_addr);

    // Create netif with default ethernet config
    esp_netif_config_t netif_config = ESP_NETIF_DEFAULT_ETH();
    self->netif = esp_netif_new(&netif_config);

    // Create glue to attach ethernet driver to netif
    self->glue = esp_eth_new_netif_glue(self->eth_handle);
    CHECK_ESP_RESULT(esp_netif_attach(self->netif, self->glue));

    // Register event handlers
    CHECK_ESP_RESULT(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, _eth_event_handler, self));
    CHECK_ESP_RESULT(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, _eth_event_handler, self));

    // Set hostname
    uint8_t local_mac[MAC_ADDRESS_LENGTH];
    esp_eth_ioctl(self->eth_handle, ETH_CMD_G_MAC_ADDR, local_mac);
    char hostname[32];
    snprintf(hostname, sizeof(hostname), "cpy-%02x%02x%02x%02x%02x%02x",
        local_mac[0], local_mac[1], local_mac[2],
        local_mac[3], local_mac[4], local_mac[5]);
    esp_netif_set_hostname(self->netif, hostname);

    // Start the ethernet driver (DHCP starts automatically with default netif config)
    CHECK_ESP_RESULT(esp_eth_start(self->eth_handle));
    self->started = true;
}

bool common_hal_mii_ethernet_get_enabled(mii_ethernet_obj_t *self) {
    return self->started;
}

void common_hal_mii_ethernet_set_enabled(mii_ethernet_obj_t *self, bool enabled) {
    if (self->eth_handle == NULL) {
        return;
    }
    if (self->started && !enabled) {
        esp_netif_dhcpc_stop(self->netif);
        esp_eth_stop(self->eth_handle);
        self->started = false;
    } else if (!self->started && enabled) {
        CHECK_ESP_RESULT(esp_eth_start(self->eth_handle));
        self->started = true;
        esp_netif_dhcpc_start(self->netif);
    }
}

bool common_hal_mii_ethernet_get_connected(mii_ethernet_obj_t *self) {
    return self->started && self->connected;
}

mp_obj_t common_hal_mii_ethernet_get_hostname(mii_ethernet_obj_t *self) {
    if (self->netif == NULL) {
        return mp_obj_new_str("", 0);
    }
    const char *hostname = NULL;
    esp_netif_get_hostname(self->netif, &hostname);
    if (hostname == NULL) {
        return mp_obj_new_str("", 0);
    }
    return mp_obj_new_str(hostname, strlen(hostname));
}

void common_hal_mii_ethernet_set_hostname(mii_ethernet_obj_t *self, const char *hostname) {
    if (self->netif == NULL) {
        return;
    }
    CHECK_ESP_RESULT(esp_netif_set_hostname(self->netif, hostname));
}

const char *mii_get_hostname_raw(void) {
    mii_ethernet_obj_t *self = (mii_ethernet_obj_t *)mii_get_default_instance();
    if (self == NULL || self->netif == NULL) {
        return "";
    }
    const char *hostname = NULL;
    esp_netif_get_hostname(self->netif, &hostname);
    return hostname == NULL ? "" : hostname;
}

// Raw uint32_t IPv4 address of the interface, in network byte order, or 0
// when the interface has none. Used by supervisor/shared/web_workflow.
uint32_t mii_get_ipv4_address(void) {
    mii_ethernet_obj_t *self = (mii_ethernet_obj_t *)mii_get_default_instance();
    if (self == NULL || self->netif == NULL || !esp_netif_is_netif_up(self->netif)) {
        return 0;
    }
    self->ip_info.ip.addr = 0;
    esp_netif_get_ip_info(self->netif, &self->ip_info);
    return self->ip_info.ip.addr;
}

// Blocks until the interface has an IPv4 address (DHCP lease or static), or
// 20s pass, or a hard interrupt arrives. Returns immediately when the
// interface cannot ever get one (down, disabled or no link). Used by the
// supervisor so the web workflow's listening socket can be created during
// the boot that brings the interface up instead of after the next VM start.
void mii_wait_for_lease(void) {
    mii_ethernet_obj_t *self = (mii_ethernet_obj_t *)mii_get_default_instance();
    if (self == NULL || self->netif == NULL || !self->started || !self->connected) {
        return;
    }
    int64_t deadline = esp_timer_get_time() + 20 * 1000000;
    while (esp_timer_get_time() < deadline && mii_get_ipv4_address() == 0) {
        if (mp_hal_is_interrupted()) {
            break;
        }
        RUN_BACKGROUND_TASKS;
        port_task_sleep_ms(50);
    }
}

mp_obj_t common_hal_mii_ethernet_get_mac_address(mii_ethernet_obj_t *self) {
    uint8_t mac[MAC_ADDRESS_LENGTH] = {0};
    if (self->eth_handle != NULL) {
        esp_eth_ioctl(self->eth_handle, ETH_CMD_G_MAC_ADDR, mac);
    }
    return mp_obj_new_bytes(mac, MAC_ADDRESS_LENGTH);
}

mp_int_t common_hal_mii_ethernet_get_link_speed(mii_ethernet_obj_t *self) {
    if (self->eth_handle == NULL || !self->connected) {
        return 0;
    }
    eth_speed_t speed;
    if (esp_eth_ioctl(self->eth_handle, ETH_CMD_G_SPEED, &speed) != ESP_OK) {
        return 0;
    }
    return speed == ETH_SPEED_100M ? 100 : 10;
}

bool common_hal_mii_ethernet_get_full_duplex(mii_ethernet_obj_t *self) {
    if (self->eth_handle == NULL || !self->connected) {
        return false;
    }
    eth_duplex_t duplex;
    if (esp_eth_ioctl(self->eth_handle, ETH_CMD_G_DUPLEX_MODE, &duplex) != ESP_OK) {
        return false;
    }
    return duplex == ETH_DUPLEX_FULL;
}

void common_hal_mii_ethernet_start_dhcp_client(mii_ethernet_obj_t *self, bool ipv4, bool ipv6) {
    if (self->netif == NULL) {
        return;
    }
    if (ipv4) {
        esp_netif_dhcpc_start(self->netif);
    } else {
        esp_netif_dhcpc_stop(self->netif);
    }
    if (ipv6) {
        // TODO: DHCPv6 support
    }
}

void common_hal_mii_ethernet_stop_dhcp_client(mii_ethernet_obj_t *self) {
    if (self->netif == NULL) {
        return;
    }
    esp_netif_dhcpc_stop(self->netif);
}

mp_obj_t common_hal_mii_ethernet_get_ipv4_gateway(mii_ethernet_obj_t *self) {
    if (self->netif == NULL || !esp_netif_is_netif_up(self->netif)) {
        return mp_const_none;
    }
    esp_netif_get_ip_info(self->netif, &self->ip_info);
    return common_hal_ipaddress_new_ipv4address(self->ip_info.gw.addr);
}

mp_obj_t common_hal_mii_ethernet_get_ipv4_subnet(mii_ethernet_obj_t *self) {
    if (self->netif == NULL || !esp_netif_is_netif_up(self->netif)) {
        return mp_const_none;
    }
    esp_netif_get_ip_info(self->netif, &self->ip_info);
    return common_hal_ipaddress_new_ipv4address(self->ip_info.netmask.addr);
}

mp_obj_t common_hal_mii_mii_get_ipv4_address(mii_ethernet_obj_t *self) {
    if (self->netif == NULL || !esp_netif_is_netif_up(self->netif)) {
        return mp_const_none;
    }
    esp_netif_get_ip_info(self->netif, &self->ip_info);
    return common_hal_ipaddress_new_ipv4address(self->ip_info.ip.addr);
}

mp_obj_t common_hal_mii_ethernet_get_ipv4_dns(mii_ethernet_obj_t *self) {
    if (self->netif == NULL || !esp_netif_is_netif_up(self->netif)) {
        return mp_const_none;
    }
    esp_netif_get_dns_info(self->netif, ESP_NETIF_DNS_MAIN, &self->dns_info);
    if (self->dns_info.ip.type != ESP_IPADDR_TYPE_V4) {
        return mp_const_none;
    }
    return common_hal_ipaddress_new_ipv4address(self->dns_info.ip.u_addr.ip4.addr);
}

void common_hal_mii_ethernet_set_ipv4_dns(mii_ethernet_obj_t *self, mp_obj_t ipv4_dns_addr) {
    if (self->netif == NULL) {
        return;
    }
    esp_netif_dns_info_t dns_addr;
    _ipv4address_to_esp_idf(ipv4_dns_addr, &dns_addr.ip.u_addr.ip4);
    esp_netif_set_dns_info(self->netif, ESP_NETIF_DNS_MAIN, &dns_addr);
}

mp_obj_t common_hal_mii_ethernet_get_addresses(mii_ethernet_obj_t *self) {
    if (self->netif == NULL || !esp_netif_is_netif_up(self->netif)) {
        return mp_const_empty_tuple;
    }
    esp_netif_ip_info_t ip_info;
    esp_netif_get_ip_info(self->netif, &ip_info);
    if (ip_info.ip.addr == IPADDR_ANY) {
        return mp_const_empty_tuple;
    }
    mp_obj_t addr = _ip4_to_str(&ip_info.ip);
    return mp_obj_new_tuple(1, &addr);
}

mp_obj_t common_hal_mii_ethernet_get_dns(mii_ethernet_obj_t *self) {
    if (self->netif == NULL || !esp_netif_is_netif_up(self->netif)) {
        return mp_const_empty_tuple;
    }
    esp_netif_dns_info_t dns_info;
    esp_netif_get_dns_info(self->netif, ESP_NETIF_DNS_MAIN, &dns_info);
    if (dns_info.ip.type == ESP_IPADDR_TYPE_V4 && dns_info.ip.u_addr.ip4.addr == IPADDR_ANY) {
        return mp_const_empty_tuple;
    }
    mp_obj_t addr = _ipaddr_to_str(&dns_info.ip);
    return mp_obj_new_tuple(1, &addr);
}

void common_hal_mii_ethernet_set_dns(mii_ethernet_obj_t *self, mp_obj_t dns_addrs_obj) {
    (void)self;
    (void)dns_addrs_obj;
    mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q"), MP_QSTR_dns);
}

void common_hal_mii_ethernet_set_ipv4_address(mii_ethernet_obj_t *self, mp_obj_t ipv4, mp_obj_t netmask, mp_obj_t gateway, mp_obj_t ipv4_dns) {
    if (self->netif == NULL) {
        return;
    }
    common_hal_mii_ethernet_stop_dhcp_client(self);

    esp_netif_ip_info_t ip_info;
    _ipv4address_to_esp_idf(ipv4, &ip_info.ip);
    _ipv4address_to_esp_idf(netmask, &ip_info.netmask);
    _ipv4address_to_esp_idf(gateway, &ip_info.gw);

    esp_netif_set_ip_info(self->netif, &ip_info);

    if (ipv4_dns != MP_OBJ_NULL) {
        common_hal_mii_ethernet_set_ipv4_dns(self, ipv4_dns);
    }
}

mp_int_t common_hal_mii_ethernet_ping(mii_ethernet_obj_t *self, mp_obj_t ip_address, mp_float_t timeout) {
    (void)self;
    (void)ip_address;
    (void)timeout;
    mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q"), MP_QSTR_ping);
}
