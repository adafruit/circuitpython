//    Copyright 2017 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.

#ifndef _UXR_CLIENT_CONFIG_H_
#define _UXR_CLIENT_CONFIG_H_

#define UXR_CLIENT_VERSION_MAJOR 2
#define UXR_CLIENT_VERSION_MINOR 4
#define UXR_CLIENT_VERSION_MICRO 2
#define UXR_CLIENT_VERSION_STR "2.4.2"

#define UCLIENT_PROFILE_DISCOVERY

#define UCLIENT_PROFILE_UDP
/* #undef UCLIENT_PROFILE_TCP */
/* #undef UCLIENT_PROFILE_CAN */
/* #undef UCLIENT_PROFILE_SERIAL */
#define UCLIENT_PROFILE_CUSTOM_TRANSPORT
/* #undef UCLIENT_PROFILE_MULTITHREAD */
/* #undef UCLIENT_PROFILE_SHARED_MEMORY */
#define UCLIENT_PROFILE_STREAM_FRAMING

/* #undef UCLIENT_PLATFORM_POSIX */
#define UCLIENT_PLATFORM_POSIX_NOPOLL
/* #undef UCLIENT_PLATFORM_WINDOWS */
/* #undef UCLIENT_PLATFORM_FREERTOS_PLUS_TCP */
/* #undef UCLIENT_PLATFORM_RTEMS_BSD_NET */
/* #undef UCLIENT_PLATFORM_ZEPHYR */

#define UXR_CONFIG_MAX_OUTPUT_BEST_EFFORT_STREAMS     1
#define UXR_CONFIG_MAX_OUTPUT_RELIABLE_STREAMS        1
#define UXR_CONFIG_MAX_INPUT_BEST_EFFORT_STREAMS      1
#define UXR_CONFIG_MAX_INPUT_RELIABLE_STREAMS         1

#define UXR_CONFIG_MAX_SESSION_CONNECTION_ATTEMPTS    10
#define UXR_CONFIG_MIN_SESSION_CONNECTION_INTERVAL    1000
#define UXR_CONFIG_MIN_HEARTBEAT_TIME_INTERVAL        1

#ifdef UCLIENT_PROFILE_UDP
#define UXR_CONFIG_UDP_TRANSPORT_MTU                  512
#endif
#ifdef UCLIENT_PROFILE_TCP
#define UXR_CONFIG_TCP_TRANSPORT_MTU                  512
#endif
#ifdef UCLIENT_PROFILE_SERIAL
#define UXR_CONFIG_SERIAL_TRANSPORT_MTU               512
#endif
#ifdef UCLIENT_PROFILE_CUSTOM_TRANSPORT
#define UXR_CONFIG_CUSTOM_TRANSPORT_MTU               512
#endif

#ifdef UCLIENT_PROFILE_SHARED_MEMORY
#define UXR_CONFIG_SHARED_MEMORY_MAX_ENTITIES          4
#define UXR_CONFIG_SHARED_MEMORY_STATIC_MEM_SIZE       10
#endif

#define UCLIENT_TWEAK_XRCE_WRITE_LIMIT

/* #undef UCLIENT_HARD_LIVELINESS_CHECK */

#ifdef UCLIENT_HARD_LIVELINESS_CHECK
#define UXR_CONFIG_HARD_LIVELINESS_CHECK_TIMEOUT_STR     "10000"
#endif


// Version checks
#if UXR_CLIENT_VERSION_MAJOR >= 3
#error UCLIENT_HARD_LIVELINESS_CHECK shall be included in session API
#error MTU must be included in CREATE_CLIENT_Payload properties
#error Reorder ObjectInfo https://github.com/eProsima/Micro-XRCE-DDS/issues/137
#endif

#endif // _UXR_CLIENT_CONFIG_H_
