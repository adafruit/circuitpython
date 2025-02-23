// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from stereo_msgs:msg/DisparityImage.idl
// generated code does not contain a copyright notice

#include "stereo_msgs/msg/detail/disparity_image__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_stereo_msgs
const rosidl_type_hash_t *
stereo_msgs__msg__DisparityImage__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x1e, 0xc1, 0xff, 0x6b, 0x5b, 0xac, 0xe9, 0x19,
      0xe4, 0x54, 0x4a, 0x37, 0xf2, 0xd9, 0x6e, 0xad,
      0x9f, 0x81, 0x78, 0x37, 0x01, 0xb7, 0xb7, 0xa4,
      0xd9, 0x7a, 0x09, 0x32, 0x5e, 0xcf, 0x27, 0x11,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "std_msgs/msg/detail/header__functions.h"
#include "sensor_msgs/msg/detail/image__functions.h"
#include "sensor_msgs/msg/detail/region_of_interest__functions.h"
#include "builtin_interfaces/msg/detail/time__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t builtin_interfaces__msg__Time__EXPECTED_HASH = {1, {
    0xb1, 0x06, 0x23, 0x5e, 0x25, 0xa4, 0xc5, 0xed,
    0x35, 0x09, 0x8a, 0xa0, 0xa6, 0x1a, 0x3e, 0xe9,
    0xc9, 0xb1, 0x8d, 0x19, 0x7f, 0x39, 0x8b, 0x0e,
    0x42, 0x06, 0xce, 0xa9, 0xac, 0xf9, 0xc1, 0x97,
  }};
static const rosidl_type_hash_t sensor_msgs__msg__Image__EXPECTED_HASH = {1, {
    0xd3, 0x1d, 0x41, 0xa9, 0xa4, 0xc4, 0xbc, 0x8e,
    0xae, 0x9b, 0xe7, 0x57, 0xb0, 0xbe, 0xed, 0x30,
    0x65, 0x64, 0xf7, 0x52, 0x6c, 0x88, 0xea, 0x6a,
    0x45, 0x88, 0xfb, 0x95, 0x82, 0x52, 0x7d, 0x47,
  }};
static const rosidl_type_hash_t sensor_msgs__msg__RegionOfInterest__EXPECTED_HASH = {1, {
    0xad, 0x16, 0xbc, 0xba, 0x5f, 0x91, 0x31, 0xdc,
    0xdb, 0xa6, 0xfb, 0xde, 0xd1, 0x9f, 0x72, 0x6f,
    0x54, 0x40, 0xe3, 0xc5, 0x13, 0xb4, 0xfb, 0x58,
    0x6d, 0xd3, 0x02, 0x7e, 0xee, 0xd8, 0xab, 0xb1,
  }};
static const rosidl_type_hash_t std_msgs__msg__Header__EXPECTED_HASH = {1, {
    0xf4, 0x9f, 0xb3, 0xae, 0x2c, 0xf0, 0x70, 0xf7,
    0x93, 0x64, 0x5f, 0xf7, 0x49, 0x68, 0x3a, 0xc6,
    0xb0, 0x62, 0x03, 0xe4, 0x1c, 0x89, 0x1e, 0x17,
    0x70, 0x1b, 0x1c, 0xb5, 0x97, 0xce, 0x6a, 0x01,
  }};
#endif

static char stereo_msgs__msg__DisparityImage__TYPE_NAME[] = "stereo_msgs/msg/DisparityImage";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char sensor_msgs__msg__Image__TYPE_NAME[] = "sensor_msgs/msg/Image";
static char sensor_msgs__msg__RegionOfInterest__TYPE_NAME[] = "sensor_msgs/msg/RegionOfInterest";
static char std_msgs__msg__Header__TYPE_NAME[] = "std_msgs/msg/Header";

// Define type names, field names, and default values
static char stereo_msgs__msg__DisparityImage__FIELD_NAME__header[] = "header";
static char stereo_msgs__msg__DisparityImage__FIELD_NAME__image[] = "image";
static char stereo_msgs__msg__DisparityImage__FIELD_NAME__f[] = "f";
static char stereo_msgs__msg__DisparityImage__FIELD_NAME__t[] = "t";
static char stereo_msgs__msg__DisparityImage__FIELD_NAME__valid_window[] = "valid_window";
static char stereo_msgs__msg__DisparityImage__FIELD_NAME__min_disparity[] = "min_disparity";
static char stereo_msgs__msg__DisparityImage__FIELD_NAME__max_disparity[] = "max_disparity";
static char stereo_msgs__msg__DisparityImage__FIELD_NAME__delta_d[] = "delta_d";

static rosidl_runtime_c__type_description__Field stereo_msgs__msg__DisparityImage__FIELDS[] = {
  {
    {stereo_msgs__msg__DisparityImage__FIELD_NAME__header, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {std_msgs__msg__Header__TYPE_NAME, 19, 19},
    },
    {NULL, 0, 0},
  },
  {
    {stereo_msgs__msg__DisparityImage__FIELD_NAME__image, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {sensor_msgs__msg__Image__TYPE_NAME, 21, 21},
    },
    {NULL, 0, 0},
  },
  {
    {stereo_msgs__msg__DisparityImage__FIELD_NAME__f, 1, 1},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {stereo_msgs__msg__DisparityImage__FIELD_NAME__t, 1, 1},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {stereo_msgs__msg__DisparityImage__FIELD_NAME__valid_window, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {sensor_msgs__msg__RegionOfInterest__TYPE_NAME, 32, 32},
    },
    {NULL, 0, 0},
  },
  {
    {stereo_msgs__msg__DisparityImage__FIELD_NAME__min_disparity, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {stereo_msgs__msg__DisparityImage__FIELD_NAME__max_disparity, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {stereo_msgs__msg__DisparityImage__FIELD_NAME__delta_d, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription stereo_msgs__msg__DisparityImage__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__Image__TYPE_NAME, 21, 21},
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__RegionOfInterest__TYPE_NAME, 32, 32},
    {NULL, 0, 0},
  },
  {
    {std_msgs__msg__Header__TYPE_NAME, 19, 19},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
stereo_msgs__msg__DisparityImage__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {stereo_msgs__msg__DisparityImage__TYPE_NAME, 30, 30},
      {stereo_msgs__msg__DisparityImage__FIELDS, 8, 8},
    },
    {stereo_msgs__msg__DisparityImage__REFERENCED_TYPE_DESCRIPTIONS, 4, 4},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&sensor_msgs__msg__Image__EXPECTED_HASH, sensor_msgs__msg__Image__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = sensor_msgs__msg__Image__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&sensor_msgs__msg__RegionOfInterest__EXPECTED_HASH, sensor_msgs__msg__RegionOfInterest__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = sensor_msgs__msg__RegionOfInterest__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&std_msgs__msg__Header__EXPECTED_HASH, std_msgs__msg__Header__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = std_msgs__msg__Header__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# Separate header for compatibility with current TimeSynchronizer.\n"
  "# Likely to be removed in a later release, use image.header instead.\n"
  "std_msgs/Header header\n"
  "\n"
  "# Floating point disparity image. The disparities are pre-adjusted for any\n"
  "# x-offset between the principal points of the two cameras (in the case\n"
  "# that they are verged). That is: d = x_l - x_r - (cx_l - cx_r)\n"
  "sensor_msgs/Image image\n"
  "\n"
  "# Stereo geometry. For disparity d, the depth from the camera is Z = fT/d.\n"
  "float32 f # Focal length, pixels\n"
  "float32 t # Baseline, world units\n"
  "\n"
  "# Subwindow of (potentially) valid disparity values.\n"
  "sensor_msgs/RegionOfInterest valid_window\n"
  "\n"
  "# The range of disparities searched.\n"
  "# In the disparity image, any disparity less than min_disparity is invalid.\n"
  "# The disparity search range defines the horopter, or 3D volume that the\n"
  "# stereo algorithm can \"see\". Points with Z outside of:\n"
  "#     Z_min = fT / max_disparity\n"
  "#     Z_max = fT / min_disparity\n"
  "# could not be found.\n"
  "float32 min_disparity\n"
  "float32 max_disparity\n"
  "\n"
  "# Smallest allowed disparity increment. The smallest achievable depth range\n"
  "# resolution is delta_Z = (Z^2/fT)*delta_d.\n"
  "float32 delta_d";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
stereo_msgs__msg__DisparityImage__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {stereo_msgs__msg__DisparityImage__TYPE_NAME, 30, 30},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 1146, 1146},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
stereo_msgs__msg__DisparityImage__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[5];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 5, 5};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *stereo_msgs__msg__DisparityImage__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *sensor_msgs__msg__Image__get_individual_type_description_source(NULL);
    sources[3] = *sensor_msgs__msg__RegionOfInterest__get_individual_type_description_source(NULL);
    sources[4] = *std_msgs__msg__Header__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
