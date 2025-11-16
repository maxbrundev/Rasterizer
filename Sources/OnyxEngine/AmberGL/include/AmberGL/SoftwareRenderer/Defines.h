#pragma once

constexpr uint8_t AGL_TEXTURE0 = 0;

// State flags (bit flags for enabling/disabling features)
#define AGL_DEPTH_WRITE 0x0001
#define AGL_COLOR_WRITE 0x0002
#define AGL_DEPTH_TEST  0x0004
#define AGL_CULL_FACE   0x0008
#define AGL_MULTISAMPLE 0x0010

// Buffer bits (also bit flags, but for a different purpose)
#define AGL_COLOR_BUFFER_BIT 0x0010
#define AGL_DEPTH_BUFFER_BIT 0x0020

// Primitive types (0x0100 range)
#define AGL_TRIANGLES      0x0100
#define AGL_LINES          0x0101
#define AGL_POINTS         0x0102
#define AGL_TRIANGLE_STRIP 0x0103

// Polygon modes (0x0200 range)
#define AGL_FILL  0x0200
#define AGL_LINE  0x0201
#define AGL_POINT 0x0202

// Face-culling modes (0x0300 range)
#define AGL_BACK           0x0300
#define AGL_FRONT          0x0301
#define AGL_FRONT_AND_BACK 0x0302

// State values (0x0400 range)
#define AGL_SAMPLES 0x0400

// Buffer binding targets (0x0500 range)
#define AGL_ARRAY_BUFFER         0x0500
#define AGL_ELEMENT_ARRAY_BUFFER 0x0501
#define AGL_FRAMEBUFFER          0x0502

// Texture targets (0x0600 range)
#define AGL_TEXTURE_2D 0x0600

// Texture parameters (0x0700 range)
#define AGL_TEXTURE_MIN_FILTER 0x0700
#define AGL_TEXTURE_MAG_FILTER 0x0701
#define AGL_TEXTURE_WRAP_S     0x0702
#define AGL_TEXTURE_WRAP_T     0x0703

// Texture/pixel formats (0x0800 range)
#define AGL_DEPTH_COMPONENT 0x0800
#define AGL_RGBA8           0x0801
#define AGL_R32F            0x0802  // ADD THIS - 32-bit float single channel
#define AGL_FLOAT           0x0803
#define AGL_UNSIGNED_BYTE   0x0804

// Wrap modes (0x0900 range)
#define AGL_CLAMP  0x0900
#define AGL_REPEAT 0x0901

// Filtering modes (0x0A00 range)
#define AGL_NEAREST                0x0A00
#define AGL_LINEAR                 0x0A01
#define AGL_NEAREST_MIPMAP_NEAREST 0x0A02
#define AGL_LINEAR_MIPMAP_LINEAR   0x0A03
#define AGL_LINEAR_MIPMAP_NEAREST  0x0A04
#define AGL_NEAREST_MIPMAP_LINEAR  0x0A05

// Depth functions (0x0B00 range)
#define AGL_NEVER    0x0B00
#define AGL_LESS     0x0B01
#define AGL_EQUAL    0x0B02
#define AGL_LEQUAL   0x0B03
#define AGL_GREATER  0x0B04
#define AGL_NOTEQUAL 0x0B05
#define AGL_GEQUAL   0x0B06
#define AGL_ALWAYS   0x0B07

// Framebuffer attachments (0x0C00 range)
#define AGL_COLOR_ATTACHMENT 0x0C00
#define AGL_DEPTH_ATTACHMENT 0x0C01

// Viewport (0x0D00 range)
#define AGL_VIEWPORT 0x0D00

#define AGL_LINE_WIDTH 0x0E00
#define AGL_POINT_SIZE 0x0E01
#define AGL_DEPTH_FUNC 0x0E02
#define AGL_POLYGON 0x0E03

#define AGL_STENCIL_TEST 0x0040
#define AGL_STENCIL_BUFFER_BIT 0x0080

#define AGL_KEEP      0x0F00
#define AGL_ZERO      0x0F01
#define AGL_REPLACE   0x0F02
#define AGL_INCR      0x0F03
#define AGL_INCR_WRAP 0x0F04
#define AGL_DECR      0x0F05
#define AGL_DECR_WRAP 0x0F06
#define AGL_INVERT    0x0F07

#define AGL_STENCIL_FUNC 0x1000
#define AGL_STENCIL_REF  0x1001
#define AGL_STENCIL_MASK 0x1002
#define AGL_STENCIL_FAIL 0x1003
#define AGL_STENCIL_PASS_DEPTH_FAIL 0x1004
#define AGL_STENCIL_PASS_DEPTH_PASS 0x1005