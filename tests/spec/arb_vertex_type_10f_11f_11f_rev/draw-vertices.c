/*
 * Copyright © 2013 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/**
 * \file api-errors.c
 *
 * This tests a few error conditions from the ARB_vertex_type_10f_11f_11f_rev extension.
 *
 * UF10/UF11 packing code based on The OpenGL Programming Guide / 7th Edition, Appendix J,
 * with bugfix from gallium source.
 */

#include "piglit-util-gl-common.h"

PIGLIT_GL_TEST_CONFIG_BEGIN
	config.supports_gl_compat_version = 30;
	config.window_visual = PIGLIT_GL_VISUAL_RGB | PIGLIT_GL_VISUAL_DOUBLE;
	config.window_width = 128;
	config.window_height = 128;
PIGLIT_GL_TEST_CONFIG_END


#define UF11(e, m)           ((e << 6) | (m))
#define UF11_EXPONENT_BIAS   15
#define UF11_EXPONENT_BITS   0x1F
#define UF11_EXPONENT_SHIFT  6
#define UF11_MANTISSA_BITS   0x3F
#define UF11_MANTISSA_SHIFT  (23 - UF11_EXPONENT_SHIFT)
#define UF11_MAX_EXPONENT    (UF11_EXPONENT_BITS << UF11_EXPONENT_SHIFT)

#define UF10(e, m)           ((e << 5) | (m))
#define UF10_EXPONENT_BIAS   15
#define UF10_EXPONENT_BITS   0x1F
#define UF10_EXPONENT_SHIFT  5
#define UF10_MANTISSA_BITS   0x1F
#define UF10_MANTISSA_SHIFT  (23 - UF10_EXPONENT_SHIFT)
#define UF10_MAX_EXPONENT    (UF10_EXPONENT_BITS << UF10_EXPONENT_SHIFT)

#define F32_INFINITY         0x7f800000

static unsigned f32_to_uf11(float val)
{
   union {
      float f;
      uint32_t ui;
   } f32 = {val};

   uint16_t uf11 = 0;

   /* Decode little-endian 32-bit floating-point value */
   int sign = (f32.ui >> 16) & 0x8000;
   /* Map exponent to the range [-127,128] */
   int exponent = ((f32.ui >> 23) & 0xff) - 127;
   int mantissa = f32.ui & 0x007fffff;

   if (exponent == 128) { /* Infinity or NaN */
      /* From the GL_EXT_packed_float spec:
       *
       *     "Additionally: negative infinity is converted to zero; positive
       *      infinity is converted to positive infinity; and both positive and
       *      negative NaN are converted to positive NaN."
       */
      uf11 = UF11_MAX_EXPONENT;
      if (mantissa) {
         uf11 |= 1; /* NaN */
      } else {
         if (sign)
            uf11 = 0; /* 0.0 */
      }
   } else if (sign) {
      return 0;
   } else if (val > 65024.0f) {
      /* From the GL_EXT_packed_float spec:
       *
       *     "Likewise, finite positive values greater than 65024 (the maximum
       *      finite representable unsigned 11-bit floating-point value) are
       *      converted to 65024."
       */
      uf11 = UF11(30, 63);
   }
   else if (exponent > -15) { /* Representable value */
      exponent += UF11_EXPONENT_BIAS;
      mantissa >>= UF11_MANTISSA_SHIFT;
      uf11 = exponent << UF11_EXPONENT_SHIFT | mantissa;
   }

   return uf11;
}

static unsigned f32_to_uf10(float val)
{
   union {
      float f;
      uint32_t ui;
   } f32 = {val};

   uint16_t uf10 = 0;

   /* Decode little-endian 32-bit floating-point value */
   int sign = (f32.ui >> 16) & 0x8000;
   /* Map exponent to the range [-127,128] */
   int exponent = ((f32.ui >> 23) & 0xff) - 127;
   int mantissa = f32.ui & 0x007fffff;

   if (exponent == 128) {
      /* From the GL_EXT_packed_float spec:
       *
       *     "Additionally: negative infinity is converted to zero; positive
       *      infinity is converted to positive infinity; and both positive and
       *      negative NaN are converted to positive NaN."
       */
      uf10 = UF10_MAX_EXPONENT;
      if (mantissa) {
         uf10 |= 1; /* NaN */
      } else {
         if (sign)
            uf10 = 0; /* 0.0 */
      }
   } else if (sign) {
      return 0;
   } else if (val > 64512.0f) {
      /* From the GL_EXT_packed_float spec:
       *
       *     "Likewise, finite positive values greater than 64512 (the maximum
       *      finite representable unsigned 10-bit floating-point value) are
       *      converted to 64512."
       */
      uf10 = UF10(30, 31);
   }
   else if (exponent > -15) { /* Representable value */
      exponent += UF10_EXPONENT_BIAS;
      mantissa >>= UF10_MANTISSA_SHIFT;
      uf10 = exponent << UF10_EXPONENT_SHIFT | mantissa;
   }

   return uf10;
}

static unsigned float3_to_r11g11b10f(const float rgb[3])
{
   return ( f32_to_uf11(rgb[0]) & 0x7ff) |
          ((f32_to_uf11(rgb[1]) & 0x7ff) << 11) |
          ((f32_to_uf10(rgb[2]) & 0x3ff) << 22);
}

static float unpacked_pos[][2] = {
	{ -1, 0 },
	{ -0.5f, 0 },
	{ -1, 0.5f },

	{ -0.5f, 0 },
	{ 0, 0 },
	{ -0.5f, 0.5f },

	{ 0, 0 },
	{ 0.5f, 0 },
	{ 0, 0.5f },

	{ 0.5f, 0 },
	{ 1, 0 },
	{ 0.5f, 0.5f },
};

static float unpacked_colors[][3] = {
	{ 0.5, 0, 1 },
	{ 0.5, 0, 0 },
	{ 0, 0.5, 0 },
	{ 1, 0.5, 0 },
};

enum piglit_result
piglit_display()
{
	bool pass = true;
	glClearColor(0, 0, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, 12);

	piglit_present_results();

	pass = piglit_probe_pixel_rgb(8, 70, unpacked_colors[0]) && pass;
	pass = piglit_probe_pixel_rgb(40, 70, unpacked_colors[1]) && pass;
	pass = piglit_probe_pixel_rgb(72, 70, unpacked_colors[2]) && pass;
	pass = piglit_probe_pixel_rgb(104, 70, unpacked_colors[3]) && pass;

	return pass ? PIGLIT_PASS : PIGLIT_FAIL;
}

void
piglit_init(int argc, char **argv)
{
	GLuint bo_pos, bo_color, prog;
	unsigned int verts[12];
	int n;

	for (n = 0; n < 12; n++)
		verts[n] = float3_to_r11g11b10f(unpacked_colors[n/3]);

	piglit_require_extension("GL_ARB_vertex_type_2_10_10_10_rev");
	piglit_require_extension("GL_ARB_vertex_type_10f_11f_11f_rev");

	glGenBuffers(1, &bo_pos);
	glBindBuffer(GL_ARRAY_BUFFER, bo_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(unpacked_pos), unpacked_pos, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid const *)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &bo_color);
	glBindBuffer(GL_ARRAY_BUFFER, bo_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	glVertexAttribPointer(1, 3, GL_UNSIGNED_INT_10F_11F_11F_REV, GL_FALSE, 0, (GLvoid const *)0);
	glEnableVertexAttribArray(1);

	prog = piglit_build_simple_program(
		"#version 130\n"
		"#extension GL_ARB_explicit_attrib_location: require\n"
		"layout(location=0) in vec2 p;\n"
		"layout(location=1) in vec3 c;\n"
		"out vec3 color;\n"
		"void main() { gl_Position = vec4(p, 0, 1); color = c; }\n",

		"#version 130\n"
		"in vec3 color;\n"
		"void main() { gl_FragColor = vec4(color,1); }\n"
		);
	if (!prog)
		piglit_report_result(PIGLIT_FAIL);
	glUseProgram(prog);
}

