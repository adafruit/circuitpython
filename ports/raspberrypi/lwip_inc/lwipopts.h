#ifndef _LWIPOPTS_EXAMPLE_COMMONH_H
#define _LWIPOPTS_EXAMPLE_COMMONH_H


// Common settings used in most of the pico_w examples
// (see https://www.nongnu.org/lwip/2_1_x/group__lwip__opts.html for details)

#include "lwip_mem.h"

// allow override in some examples
#ifndef NO_SYS
#define NO_SYS                      1
#endif
// allow override in some examples
#ifndef LWIP_SOCKET
#define LWIP_SOCKET                 0
#endif
#if PICO_CYW43_ARCH_POLL
#define MEM_LIBC_MALLOC             1
#else
// MEM_LIBC_MALLOC is incompatible with non polling versions
#define MEM_LIBC_MALLOC             0
#endif
#define MEM_ALIGNMENT               4
// MEM_USE_POOLS: mem_malloc uses pools of fixed size memory blocks. Default is 0.
#define MEM_USE_POOLS               0
// MEM_USE_POOLS_TRY_BIGGER_POOL: if one pool is empty, try the next bigger pool. Default is 0.
#define MEM_USE_POOLS_TRY_BIGGER_POOL 0
// MEMP_USE_CUSTOM_POOLS: Use custom pools defined in lwippools.h. Default is 0.
#define MEMP_USE_CUSTOM_POOLS       0
// MEMP_MEM_MALLOC: Use mem_malloc() for pool memory. Default is 0.
#define MEMP_MEM_MALLOC             1
#define MEM_CUSTOM_ALLOCATOR        1
#define MEM_CUSTOM_FREE             lwip_heap_free
#define MEM_CUSTOM_MALLOC           lwip_heap_malloc
#define MEM_CUSTOM_CALLOC           lwip_heap_calloc

// MEM_SIZE: The LWIP heap size. Memory for mem_malloc and mem_calloc are allocated from
// this heap. If MEMP_MEM_MALLOC is set to 1, memory for memp_malloc is also allocated from
// this heap; if it is 0, memory is statically pre-allocated for each pool.
// Default is 1600.
#define MEM_SIZE                    1600
// MEMP_NUM_PBUF: memp pbufs used when sending from static memory. Default is 16.
#define MEMP_NUM_PBUF               16
// MEMP_NUM_RAW_PCB: Number of raw connection PCBs. Default is 4.
#define MEMP_NUM_RAW_PCB            4
// MEMP_NUM_UDP_PCB: Number of UDP PCBs. Default is 4.
#define MEMP_NUM_UDP_PCB            4
// MEMP_NUM_TCP_PCB: Number of simultaneously active TCP connections. Default is 5.
#define MEMP_NUM_TCP_PCB            5
// MEMP_NUM_TCP_PCB_LISTEN: Number of listening TCP PCBs. Default is 8.
#define MEMP_NUM_TCP_PCB_LISTEN     8
// MEMP_NUM_TCP_SEG: Number of simultaneously queued TCP segments. Default is 16.
#define MEMP_NUM_TCP_SEG            16
// MEMP_NUM_ALTCP_PCB: Number of simultaneously active altcp connections. Default is 5.
#define MEMP_NUM_ALTCP_PCB          5
// MEMP_NUM_REASSDATA: Number of simultaneously IP packets queued for reassembly. Default is 5.
#define MEMP_NUM_REASSDATA          5
// MEMP_NUM_FRAG_PBUF: Number of simultaneously IP fragments. Default is 15.
#define MEMP_NUM_FRAG_PBUF          15
// MEMP_NUM_ARP_QUEUE: Number of simultaneously queued ARP packets. Default is 30.
#define MEMP_NUM_ARP_QUEUE          30
// MEMP_NUM_IGMP_GROUP: Number of simultaneously active IGMP groups. Default is 8.
#define MEMP_NUM_IGMP_GROUP         8
// MEMP_NUM_SYS_TIMEOUT: Number of simultaneously active timeouts.
// Use calculated default based on enabled modules.

// PBUF_POOL_SIZE: Number of pbufs in the pbuf pool. Default is 16.
#define PBUF_POOL_SIZE              16

// LWIP's default 250 ms periodic timer interval is too long, resulting in network
// performance issues. We reduce it to 25 ms giving a slow-timer of 50 ms and a
// fast-timer of 25 ms.
#define TCP_TMR_INTERVAL            25

#define LWIP_ARP                    1
#define LWIP_ETHERNET               1
#define LWIP_ICMP                   1
#define LWIP_RAW                    1

#define TCP_WND                     (8 * TCP_MSS)
#define TCP_MSS                     1460
#define TCP_SND_BUF                 (8 * TCP_MSS)
#define TCP_SND_QUEUELEN            ((4 * (TCP_SND_BUF) + (TCP_MSS - 1)) / (TCP_MSS))
#define LWIP_NETIF_STATUS_CALLBACK  1
#define LWIP_NETIF_LINK_CALLBACK    1
#define LWIP_NETIF_HOSTNAME         1
#define LWIP_NETCONN                0
#define MEM_STATS                   0
#define SYS_STATS                   0
#define MEMP_STATS                  0
#define LINK_STATS                  0
// #define ETH_PAD_SIZE                2
#define LWIP_CHKSUM_ALGORITHM       3
#define LWIP_DHCP                   1
#define LWIP_IPV4                   1
#define LWIP_TCP                    1
#define LWIP_UDP                    1
#define LWIP_DNS                    1
#define LWIP_DNS_SUPPORT_MDNS_QUERIES   1
#define LWIP_TCP_KEEPALIVE          1
#define LWIP_NETIF_TX_SINGLE_PBUF   1
#define DHCP_DOES_ARP_CHECK         0
#define LWIP_DHCP_DOES_ACD_CHECK    0
#define SO_REUSE                    1

#if CIRCUITPY_MDNS
#define LWIP_IGMP                   1
#define LWIP_MDNS_RESPONDER         1
#define LWIP_NUM_NETIF_CLIENT_DATA  1
#define LWIP_NETIF_EXT_STATUS_CALLBACK 1
#define MDNS_MAX_SECONDARY_HOSTNAMES 1
#define MEMP_NUM_SYS_TIMEOUT        (8 + 3 * (LWIP_IPV4 + LWIP_IPV6))
#define MDNS_MAX_SERVICES           25
#endif

#ifndef NDEBUG
#define LWIP_DEBUG                  1
#define LWIP_STATS                  1
#define LWIP_STATS_DISPLAY          1
#endif

#define ETHARP_DEBUG                LWIP_DBG_OFF
#define NETIF_DEBUG                 LWIP_DBG_OFF
#define PBUF_DEBUG                  LWIP_DBG_OFF
#define API_LIB_DEBUG               LWIP_DBG_OFF
#define API_MSG_DEBUG               LWIP_DBG_OFF
#define SOCKETS_DEBUG               LWIP_DBG_OFF
#define ICMP_DEBUG                  LWIP_DBG_OFF
#define INET_DEBUG                  LWIP_DBG_OFF
#define IP_DEBUG                    LWIP_DBG_OFF
#define IP_REASS_DEBUG              LWIP_DBG_OFF
#define RAW_DEBUG                   LWIP_DBG_OFF
#define MEM_DEBUG                   LWIP_DBG_OFF
#define MEMP_DEBUG                  LWIP_DBG_OFF
#define SYS_DEBUG                   LWIP_DBG_OFF
#define TCP_DEBUG                   LWIP_DBG_OFF
#define TCP_INPUT_DEBUG             LWIP_DBG_OFF
#define TCP_OUTPUT_DEBUG            LWIP_DBG_OFF
#define TCP_RTO_DEBUG               LWIP_DBG_OFF
#define TCP_CWND_DEBUG              LWIP_DBG_OFF
#define TCP_WND_DEBUG               LWIP_DBG_OFF
#define TCP_FR_DEBUG                LWIP_DBG_OFF
#define TCP_QLEN_DEBUG              LWIP_DBG_OFF
#define TCP_RST_DEBUG               LWIP_DBG_OFF
#define UDP_DEBUG                   LWIP_DBG_OFF
#define TCPIP_DEBUG                 LWIP_DBG_OFF
#define PPP_DEBUG                   LWIP_DBG_OFF
#define SLIP_DEBUG                  LWIP_DBG_OFF
#define DHCP_DEBUG                  LWIP_DBG_OFF
#define MDNS_DEBUG                  LWIP_DBG_OFF

#define LWIP_TIMEVAL_PRIVATE 0

#define LWIP_NO_CTYPE_H 1

#define X8_F  "02x"
#define U16_F "u"
#define U32_F "lu"
#define S32_F "ld"
#define X32_F "lx"

#define S16_F "d"
#define X16_F "x"
#define SZT_F "u"
#endif /* __LWIPOPTS_H__ */
