// generated from
// rosidl_typesupport_microxrcedds_c/resource/rosidl_typesupport_microxrcedds_c__visibility_control.h.in
// generated code does not contain a copyright notice

#ifndef STATISTICS_MSGS__MSG__ROSIDL_TYPESUPPORT_MICROXRCEDDS_C__VISIBILITY_CONTROL_H_
#define STATISTICS_MSGS__MSG__ROSIDL_TYPESUPPORT_MICROXRCEDDS_C__VISIBILITY_CONTROL_H_

#ifdef __cplusplus
extern "C"
{
#endif

// This logic was borrowed (then namespaced) from the examples on the gcc wiki:
//     https://gcc.gnu.org/wiki/Visibility

#if defined _WIN32 || defined __CYGWIN__
  #ifdef __GNUC__
    #define ROSIDL_TYPESUPPORT_MICROXRCEDDS_C_EXPORT_statistics_msgs __attribute__ ((dllexport))
    #define ROSIDL_TYPESUPPORT_MICROXRCEDDS_C_IMPORT_statistics_msgs __attribute__ ((dllimport))
  #else
    #define ROSIDL_TYPESUPPORT_MICROXRCEDDS_C_EXPORT_statistics_msgs __declspec(dllexport)
    #define ROSIDL_TYPESUPPORT_MICROXRCEDDS_C_IMPORT_statistics_msgs __declspec(dllimport)
  #endif
  #ifdef ROSIDL_TYPESUPPORT_MICROXRCEDDS_C_BUILDING_DLL_statistics_msgs
    #define ROSIDL_TYPESUPPORT_MICROXRCEDDS_C_PUBLIC_statistics_msgs ROSIDL_TYPESUPPORT_MICROXRCEDDS_C_EXPORT_statistics_msgs
  #else
    #define ROSIDL_TYPESUPPORT_MICROXRCEDDS_C_PUBLIC_statistics_msgs ROSIDL_TYPESUPPORT_MICROXRCEDDS_C_IMPORT_statistics_msgs
  #endif
#else
  #define ROSIDL_TYPESUPPORT_MICROXRCEDDS_C_EXPORT_statistics_msgs __attribute__ ((visibility("default")))
  #define ROSIDL_TYPESUPPORT_MICROXRCEDDS_C_IMPORT_statistics_msgs
  #if __GNUC__ >= 4
    #define ROSIDL_TYPESUPPORT_MICROXRCEDDS_C_PUBLIC_statistics_msgs __attribute__ ((visibility("default")))
  #else
    #define ROSIDL_TYPESUPPORT_MICROXRCEDDS_C_PUBLIC_statistics_msgs
  #endif
#endif

#ifdef __cplusplus
}
#endif

#endif  // STATISTICS_MSGS__MSG__ROSIDL_TYPESUPPORT_MICROXRCEDDS_C__VISIBILITY_CONTROL_H_
