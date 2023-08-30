#pragma once

enum alpha_mode {
  alpha_mode_pipeline = 0,      // per-pixel alpha allowed, POS0_ALPHA ignored
  alpha_mode_fixed = 1,         // use POS0_ALPHA() for entire sprite
  alpha_mode_fixed_nonzero = 2, // POS0_ALPHA() and per-pixel both have an effect
  alpha_mode_fixed_over_7 = 3,
};

enum hvs_pixel_format {
	/* 8bpp */
	HVS_PIXEL_FORMAT_RGB332 = 0,
	/* 16bpp */
	HVS_PIXEL_FORMAT_RGBA4444 = 1,
	HVS_PIXEL_FORMAT_RGB555 = 2,
	HVS_PIXEL_FORMAT_RGBA5551 = 3,
	HVS_PIXEL_FORMAT_RGB565 = 4,
	/* 24bpp */
	HVS_PIXEL_FORMAT_RGB888 = 5,
	HVS_PIXEL_FORMAT_RGBA6666 = 6,
	/* 32bpp */
	HVS_PIXEL_FORMAT_RGBA8888 = 7,

	HVS_PIXEL_FORMAT_YCBCR_YUV420_3PLANE = 8,
	HVS_PIXEL_FORMAT_YCBCR_YUV420_2PLANE = 9,
	HVS_PIXEL_FORMAT_YCBCR_YUV422_3PLANE = 10,
	HVS_PIXEL_FORMAT_YCBCR_YUV422_2PLANE = 11,
	HVS_PIXEL_FORMAT_H264 = 12,
	HVS_PIXEL_FORMAT_PALETTE = 13,
	HVS_PIXEL_FORMAT_YUV444_RGB = 14,
	HVS_PIXEL_FORMAT_AYUV444_RGB = 15,
	HVS_PIXEL_FORMAT_RGBA1010102 = 16,
	HVS_PIXEL_FORMAT_YCBCR_10BIT = 17,
};

struct hvs_channel {
  volatile uint32_t dispctrl;
  volatile uint32_t dispbkgnd;
  volatile uint32_t dispstat;
  // 31:30  mode
  // 29     full
  // 28     empty
  // 17:12  frame count
  // 11:0   line
  volatile uint32_t dispbase;
};

extern volatile struct hvs_channel *hvs_hw_channels;

#define CONTROL_FORMAT(n)       (n & 0xf)
#define CONTROL_END             (1<<31)
#define CONTROL_VALID           (1<<30)
#define CONTROL_WORDS(n)        (((n) & 0x3f) << 24)
#define CONTROL0_FIXED_ALPHA    (1<<19)
#define CONTROL0_HFLIP          (1<<16)
#define CONTROL0_VFLIP          (1<<15)
#define CONTROL_PIXEL_ORDER(n)  ((n & 3) << 13)
#define CONTROL_SCL1(scl)       (scl << 8)
#define CONTROL_SCL0(scl)       (scl << 5)
#define CONTROL_UNITY           (1<<4)

#define POS0_X(n) (n & 0xfff)
#define POS0_Y(n) ((n & 0xfff) << 12)
#define POS0_ALPHA(n) ((n & 0xff) << 24)

#define POS2_W(n) (n & 0xffff)
#define POS2_H(n) ((n & 0xffff) << 16)

#define HVS_PIXEL_ORDER_RGBA			0
#define HVS_PIXEL_ORDER_BGRA			1
#define HVS_PIXEL_ORDER_ARGB			2
#define HVS_PIXEL_ORDER_ABGR			3

#define HVS_PIXEL_ORDER_XBRG			0
#define HVS_PIXEL_ORDER_XRBG			1
#define HVS_PIXEL_ORDER_XRGB			2
#define HVS_PIXEL_ORDER_XBGR			3

#if BCM_VERSION == 2835
#define BCM_PERIPH_BASE_VIRT 0x20000000
#endif
#define SCALER_BASE (BCM_PERIPH_BASE_VIRT + 0x400000)

#define SCALER_DISPCTRL0    (SCALER_BASE + 0x40)
#define SCALER_DISPCTRLX_ENABLE (1<<31)
#define SCALER_DISPCTRLX_RESET  (1<<30)
#define SCALER_DISPCTRL_W(n)    ((n & 0xfff) << 12)
#define SCALER_DISPCTRL_H(n)    (n & 0xfff)

#define SCALER_DISPLIST0    (SCALER_BASE + 0x20)
#define SCALER_DISPLIST1    (SCALER_BASE + 0x24)
#define SCALER_DISPLIST2    (SCALER_BASE + 0x28)

#define SCALER_LIST_MEMORY  (SCALER_BASE + 0x2000)
#define SCALER5_LIST_MEMORY  (SCALER_BASE + 0x4000)
