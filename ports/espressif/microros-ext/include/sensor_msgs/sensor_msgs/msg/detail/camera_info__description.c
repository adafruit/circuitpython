// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from sensor_msgs:msg/CameraInfo.idl
// generated code does not contain a copyright notice

#include "sensor_msgs/msg/detail/camera_info__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_sensor_msgs
const rosidl_type_hash_t *
sensor_msgs__msg__CameraInfo__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xb3, 0xdf, 0xd6, 0x8f, 0xf4, 0x6c, 0x9d, 0x56,
      0xc8, 0x0f, 0xd3, 0xbd, 0x4e, 0xd2, 0x2c, 0x7a,
      0x4d, 0xdc, 0xe8, 0xc8, 0x34, 0x8f, 0x2f, 0x59,
      0xc2, 0x99, 0xe7, 0x31, 0x18, 0xe7, 0xe2, 0x75,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "sensor_msgs/msg/detail/region_of_interest__functions.h"
#include "std_msgs/msg/detail/header__functions.h"
#include "builtin_interfaces/msg/detail/time__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t builtin_interfaces__msg__Time__EXPECTED_HASH = {1, {
    0xb1, 0x06, 0x23, 0x5e, 0x25, 0xa4, 0xc5, 0xed,
    0x35, 0x09, 0x8a, 0xa0, 0xa6, 0x1a, 0x3e, 0xe9,
    0xc9, 0xb1, 0x8d, 0x19, 0x7f, 0x39, 0x8b, 0x0e,
    0x42, 0x06, 0xce, 0xa9, 0xac, 0xf9, 0xc1, 0x97,
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

static char sensor_msgs__msg__CameraInfo__TYPE_NAME[] = "sensor_msgs/msg/CameraInfo";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char sensor_msgs__msg__RegionOfInterest__TYPE_NAME[] = "sensor_msgs/msg/RegionOfInterest";
static char std_msgs__msg__Header__TYPE_NAME[] = "std_msgs/msg/Header";

// Define type names, field names, and default values
static char sensor_msgs__msg__CameraInfo__FIELD_NAME__header[] = "header";
static char sensor_msgs__msg__CameraInfo__FIELD_NAME__height[] = "height";
static char sensor_msgs__msg__CameraInfo__FIELD_NAME__width[] = "width";
static char sensor_msgs__msg__CameraInfo__FIELD_NAME__distortion_model[] = "distortion_model";
static char sensor_msgs__msg__CameraInfo__FIELD_NAME__d[] = "d";
static char sensor_msgs__msg__CameraInfo__FIELD_NAME__k[] = "k";
static char sensor_msgs__msg__CameraInfo__FIELD_NAME__r[] = "r";
static char sensor_msgs__msg__CameraInfo__FIELD_NAME__p[] = "p";
static char sensor_msgs__msg__CameraInfo__FIELD_NAME__binning_x[] = "binning_x";
static char sensor_msgs__msg__CameraInfo__FIELD_NAME__binning_y[] = "binning_y";
static char sensor_msgs__msg__CameraInfo__FIELD_NAME__roi[] = "roi";

static rosidl_runtime_c__type_description__Field sensor_msgs__msg__CameraInfo__FIELDS[] = {
  {
    {sensor_msgs__msg__CameraInfo__FIELD_NAME__header, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {std_msgs__msg__Header__TYPE_NAME, 19, 19},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__CameraInfo__FIELD_NAME__height, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__CameraInfo__FIELD_NAME__width, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__CameraInfo__FIELD_NAME__distortion_model, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__CameraInfo__FIELD_NAME__d, 1, 1},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__CameraInfo__FIELD_NAME__k, 1, 1},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE_ARRAY,
      9,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__CameraInfo__FIELD_NAME__r, 1, 1},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE_ARRAY,
      9,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__CameraInfo__FIELD_NAME__p, 1, 1},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE_ARRAY,
      12,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__CameraInfo__FIELD_NAME__binning_x, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__CameraInfo__FIELD_NAME__binning_y, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__CameraInfo__FIELD_NAME__roi, 3, 3},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {sensor_msgs__msg__RegionOfInterest__TYPE_NAME, 32, 32},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription sensor_msgs__msg__CameraInfo__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
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
sensor_msgs__msg__CameraInfo__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {sensor_msgs__msg__CameraInfo__TYPE_NAME, 26, 26},
      {sensor_msgs__msg__CameraInfo__FIELDS, 11, 11},
    },
    {sensor_msgs__msg__CameraInfo__REFERENCED_TYPE_DESCRIPTIONS, 3, 3},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&sensor_msgs__msg__RegionOfInterest__EXPECTED_HASH, sensor_msgs__msg__RegionOfInterest__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = sensor_msgs__msg__RegionOfInterest__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&std_msgs__msg__Header__EXPECTED_HASH, std_msgs__msg__Header__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = std_msgs__msg__Header__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# This message defines meta information for a camera. It should be in a\n"
  "# camera namespace on topic \"camera_info\" and accompanied by up to five\n"
  "# image topics named:\n"
  "#\n"
  "#   image_raw - raw data from the camera driver, possibly Bayer encoded\n"
  "#   image            - monochrome, distorted\n"
  "#   image_color      - color, distorted\n"
  "#   image_rect       - monochrome, rectified\n"
  "#   image_rect_color - color, rectified\n"
  "#\n"
  "# The image_pipeline contains packages (image_proc, stereo_image_proc)\n"
  "# for producing the four processed image topics from image_raw and\n"
  "# camera_info. The meaning of the camera parameters are described in\n"
  "# detail at http://www.ros.org/wiki/image_pipeline/CameraInfo.\n"
  "#\n"
  "# The image_geometry package provides a user-friendly interface to\n"
  "# common operations using this meta information. If you want to, e.g.,\n"
  "# project a 3d point into image coordinates, we strongly recommend\n"
  "# using image_geometry.\n"
  "#\n"
  "# If the camera is uncalibrated, the matrices D, K, R, P should be left\n"
  "# zeroed out. In particular, clients may assume that K[0] == 0.0\n"
  "# indicates an uncalibrated camera.\n"
  "\n"
  "#######################################################################\n"
  "#                     Image acquisition info                          #\n"
  "#######################################################################\n"
  "\n"
  "# Time of image acquisition, camera coordinate frame ID\n"
  "std_msgs/Header header # Header timestamp should be acquisition time of image\n"
  "                             # Header frame_id should be optical frame of camera\n"
  "                             # origin of frame should be optical center of camera\n"
  "                             # +x should point to the right in the image\n"
  "                             # +y should point down in the image\n"
  "                             # +z should point into the plane of the image\n"
  "\n"
  "\n"
  "#######################################################################\n"
  "#                      Calibration Parameters                         #\n"
  "#######################################################################\n"
  "# These are fixed during camera calibration. Their values will be the #\n"
  "# same in all messages until the camera is recalibrated. Note that    #\n"
  "# self-calibrating systems may \"recalibrate\" frequently.              #\n"
  "#                                                                     #\n"
  "# The internal parameters can be used to warp a raw (distorted) image #\n"
  "# to:                                                                 #\n"
  "#   1. An undistorted image (requires D and K)                        #\n"
  "#   2. A rectified image (requires D, K, R)                           #\n"
  "# The projection matrix P projects 3D points into the rectified image.#\n"
  "#######################################################################\n"
  "\n"
  "# The image dimensions with which the camera was calibrated.\n"
  "# Normally this will be the full camera resolution in pixels.\n"
  "uint32 height\n"
  "uint32 width\n"
  "\n"
  "# The distortion model used. Supported models are listed in\n"
  "# sensor_msgs/distortion_models.hpp. For most cameras, \"plumb_bob\" - a\n"
  "# simple model of radial and tangential distortion - is sufficent.\n"
  "string distortion_model\n"
  "\n"
  "# The distortion parameters, size depending on the distortion model.\n"
  "# For \"plumb_bob\", the 5 parameters are: (k1, k2, t1, t2, k3).\n"
  "float64[] d\n"
  "\n"
  "# Intrinsic camera matrix for the raw (distorted) images.\n"
  "#     [fx  0 cx]\n"
  "# K = [ 0 fy cy]\n"
  "#     [ 0  0  1]\n"
  "# Projects 3D points in the camera coordinate frame to 2D pixel\n"
  "# coordinates using the focal lengths (fx, fy) and principal point\n"
  "# (cx, cy).\n"
  "float64[9]  k # 3x3 row-major matrix\n"
  "\n"
  "# Rectification matrix (stereo cameras only)\n"
  "# A rotation matrix aligning the camera coordinate system to the ideal\n"
  "# stereo image plane so that epipolar lines in both stereo images are\n"
  "# parallel.\n"
  "float64[9]  r # 3x3 row-major matrix\n"
  "\n"
  "# Projection/camera matrix\n"
  "#     [fx'  0  cx' Tx]\n"
  "# P = [ 0  fy' cy' Ty]\n"
  "#     [ 0   0   1   0]\n"
  "# By convention, this matrix specifies the intrinsic (camera) matrix\n"
  "#  of the processed (rectified) image. That is, the left 3x3 portion\n"
  "#  is the normal camera intrinsic matrix for the rectified image.\n"
  "# It projects 3D points in the camera coordinate frame to 2D pixel\n"
  "#  coordinates using the focal lengths (fx', fy') and principal point\n"
  "#  (cx', cy') - these may differ from the values in K.\n"
  "# For monocular cameras, Tx = Ty = 0. Normally, monocular cameras will\n"
  "#  also have R = the identity and P[1:3,1:3] = K.\n"
  "# For a stereo pair, the fourth column [Tx Ty 0]' is related to the\n"
  "#  position of the optical center of the second camera in the first\n"
  "#  camera's frame. We assume Tz = 0 so both cameras are in the same\n"
  "#  stereo image plane. The first camera always has Tx = Ty = 0. For\n"
  "#  the right (second) camera of a horizontal stereo pair, Ty = 0 and\n"
  "#  Tx = -fx' * B, where B is the baseline between the cameras.\n"
  "# Given a 3D point [X Y Z]', the projection (x, y) of the point onto\n"
  "#  the rectified image is given by:\n"
  "#  [u v w]' = P * [X Y Z 1]'\n"
  "#         x = u / w\n"
  "#         y = v / w\n"
  "#  This holds for both images of a stereo pair.\n"
  "float64[12] p # 3x4 row-major matrix\n"
  "\n"
  "\n"
  "#######################################################################\n"
  "#                      Operational Parameters                         #\n"
  "#######################################################################\n"
  "# These define the image region actually captured by the camera       #\n"
  "# driver. Although they affect the geometry of the output image, they #\n"
  "# may be changed freely without recalibrating the camera.             #\n"
  "#######################################################################\n"
  "\n"
  "# Binning refers here to any camera setting which combines rectangular\n"
  "#  neighborhoods of pixels into larger \"super-pixels.\" It reduces the\n"
  "#  resolution of the output image to\n"
  "#  (width / binning_x) x (height / binning_y).\n"
  "# The default values binning_x = binning_y = 0 is considered the same\n"
  "#  as binning_x = binning_y = 1 (no subsampling).\n"
  "uint32 binning_x\n"
  "uint32 binning_y\n"
  "\n"
  "# Region of interest (subwindow of full camera resolution), given in\n"
  "#  full resolution (unbinned) image coordinates. A particular ROI\n"
  "#  always denotes the same window of pixels on the camera sensor,\n"
  "#  regardless of binning settings.\n"
  "# The default setting of roi (all values 0) is considered the same as\n"
  "#  full resolution (roi.width = width, roi.height = height).\n"
  "RegionOfInterest roi";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
sensor_msgs__msg__CameraInfo__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {sensor_msgs__msg__CameraInfo__TYPE_NAME, 26, 26},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 6351, 6351},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
sensor_msgs__msg__CameraInfo__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[4];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 4, 4};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *sensor_msgs__msg__CameraInfo__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *sensor_msgs__msg__RegionOfInterest__get_individual_type_description_source(NULL);
    sources[3] = *std_msgs__msg__Header__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
