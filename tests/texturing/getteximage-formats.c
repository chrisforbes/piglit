/*
 * Copyright (c) 2011 VMware, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * on the rights to use, copy, modify, merge, publish, distribute, sub
 * license, and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT.  IN NO EVENT SHALL VMWARE AND/OR THEIR SUPPLIERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


/**
 * @file getteximage-formats.c
 *
 * Test glGetTexImage with a variety of formats.
 * Brian Paul
 * Sep 2011
 */


#include "piglit-util-gl-common.h"
#include "../fbo/fbo-formats.h"

PIGLIT_GL_TEST_MAIN(
    600 /*window_width*/,
    200 /*window_height*/,
    GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE)

static const char *TestName = "getteximage-formats";

static const GLfloat clearColor[4] = { 0.4, 0.4, 0.4, 0.0 };

#define TEX_WIDTH 128
#define TEX_HEIGHT 128

#define DO_BLEND 1


/**
 * Make a simple texture image where red increases from left to right,
 * green increases from bottom to top, blue stays constant (50%) and
 * the alpha channel is a checkerboard pattern.
 * \return GL_TRUE for success, GL_FALSE if unsupported format
 */
static GLboolean
make_texture_image(GLenum intFormat, GLubyte upperRightTexel[4])
{
	GLubyte tex[TEX_HEIGHT][TEX_WIDTH][4];
	int i, j;

	for (i = 0; i < TEX_HEIGHT; i++) {
		for (j = 0; j < TEX_WIDTH; j++) {
			tex[i][j][0] = j * 255 / TEX_WIDTH;
			tex[i][j][1] = i * 255 / TEX_HEIGHT;
			tex[i][j][2] = 128;
			if (((i >> 4) ^ (j >> 4)) & 1)
				tex[i][j][3] = 255;  /* opaque */
			else
				tex[i][j][3] = 125;	/* transparent */
		}
	}

	memcpy(upperRightTexel, tex[TEX_HEIGHT-1][TEX_WIDTH-1], 4);

	glTexImage2D(GL_TEXTURE_2D, 0, intFormat, TEX_WIDTH, TEX_HEIGHT, 0,
					 GL_RGBA, GL_UNSIGNED_BYTE, tex);

	return glGetError() == GL_NO_ERROR;
}


static void
print(int x, int y, const char *s)
{
	glColor3f(1, 1, 1);
	glWindowPos2iARB(x, y);
	while (*s) {
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, (int) *s);
		s++;
	}
}


static void
draw_format_text(const struct test_desc *test,
		 const struct format_desc *fmt)
{
	char s[200];
	sprintf(s, "%s: %s", test->param, fmt->name);
	print(10, piglit_height - 20, s);
}


static void
draw_unsupported(void)
{
	print(10, piglit_height - 40, "Not supported by test");
}


static GLfloat
ubyte_to_float(GLubyte b, GLint bits)
{
	if (bits <= 8) {
		GLint b2 = b >> (8 - bits);
		GLint max = 255 >> (8 - bits);
		return b2 / (float) max;
	}
	else {
		return b / 255.0;
	}
}



static GLfloat
bits_to_tolerance(GLint bits, GLboolean compressed)
{
	GLfloat t;

	if (bits == 0) {
		return 0.25;
	}
	else if (bits == 1) {
		return 0.5;
	}
	else if (bits > 8) {
		/* The original texture was specified as GLubyte and we
		 * assume that the window/surface is 8-bits/channel.
		 */
		t = 4.0 / 255;
	}
	else {
		t = 4.0 / (1 << (bits - 1));
	}

	if (compressed) {
		/* Use a fudge factor.  The queries for GL_TEXTURE_RED/
		 * GREEN/BLUE/ALPHA_SIZE don't return well-defined values for
		 * compressed formats so using them is unreliable.  This is
		 * pretty loose, but good enough to catch some Mesa bugs during
		 * development.
		 */
		t = 0.3;
	}
	return t;
}


static void
compute_expected_color(const struct format_desc *fmt,
		       const GLubyte upperRightTexel[4],
		       GLfloat expected[4], GLfloat tolerance[4])
{
	GLfloat texel[4];
	GLint compressed;
	int bits[4];

	bits[0] = bits[1] = bits[2] = bits[3] = 0;

	/* Handle special cases first */
	if (fmt->internalformat == GL_R11F_G11F_B10F_EXT) {
		bits[0] = bits[1] = bits[2] = 8;
		bits[3] = 0;
		texel[0] = ubyte_to_float(upperRightTexel[0], bits[0]);
		texel[1] = ubyte_to_float(upperRightTexel[1], bits[1]);
		texel[2] = ubyte_to_float(upperRightTexel[2], bits[2]);
		texel[3] = 1.0;
		compressed = GL_FALSE;
	}
	else {
		GLint r, g, b, a, l, i;
		GLenum baseFormat = 0;

		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_RED_SIZE, &r);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_GREEN_SIZE, &g);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_BLUE_SIZE, &b);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_ALPHA_SIZE, &a);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_LUMINANCE_SIZE, &l);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTENSITY_SIZE, &i);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED, &compressed);

		if (0)
			printf("r=%d g=%d b=%d a=%d l=%d i=%d\n", r, g, b, a, l, i);

		if (i > 0) {
			baseFormat = GL_INTENSITY;
			bits[0] = i;
			bits[1] = 0;
			bits[2] = 0;
			bits[3] = i;
			texel[0] = ubyte_to_float(upperRightTexel[0], bits[0]);
			texel[1] = 0.0;
			texel[2] = 0.0;
			texel[3] = ubyte_to_float(upperRightTexel[0], bits[3]);
		}
		else if (a > 0) {
			if (l > 0) {
				baseFormat = GL_LUMINANCE_ALPHA;
				bits[0] = l;
				bits[1] = 0;
				bits[2] = 0;
				bits[3] = a;
				texel[0] = ubyte_to_float(upperRightTexel[0], bits[0]);
				texel[1] = 0.0;
				texel[2] = 0.0;
				texel[3] = ubyte_to_float(upperRightTexel[3], bits[3]);
			}
			else if (r > 0 && g > 0 && b > 0) {
				baseFormat = GL_RGBA;
				bits[0] = r;
				bits[1] = g;
				bits[2] = b;
				bits[3] = a;
				texel[0] = ubyte_to_float(upperRightTexel[0], bits[0]);
				texel[1] = ubyte_to_float(upperRightTexel[1], bits[1]);
				texel[2] = ubyte_to_float(upperRightTexel[2], bits[2]);
				texel[3] = ubyte_to_float(upperRightTexel[3], bits[3]);
			}
			else if (r == 0 && g == 0 && b == 0) {
				baseFormat = GL_ALPHA;
				bits[0] = 0;
				bits[1] = 0;
				bits[2] = 0;
				bits[3] = a;
				texel[0] = 0.0;
				texel[1] = 0.0;
				texel[2] = 0.0;
				texel[3] = ubyte_to_float(upperRightTexel[3], bits[3]);
			}
			else {
				baseFormat = 0;  /* ??? */
				texel[0] = 0.0;
				texel[1] = 0.0;
				texel[2] = 0.0;
				texel[3] = 0.0;
			}
		}
		else if (l > 0) {
			baseFormat = GL_LUMINANCE;
			bits[0] = l;
			bits[1] = 0;
			bits[2] = 0;
			bits[3] = 0;
			texel[0] = ubyte_to_float(upperRightTexel[0], bits[0]);
			texel[1] = 0.0;
			texel[2] = 0.0;
			texel[3] = 1.0;
		}
		else if (r > 0) {
			if (g > 0) {
				if (b > 0) {
					baseFormat = GL_RGB;
					bits[0] = r;
					bits[1] = g;
					bits[2] = b;
					bits[3] = 0;
					texel[0] = ubyte_to_float(upperRightTexel[0], bits[0]);
					texel[1] = ubyte_to_float(upperRightTexel[1], bits[1]);
					texel[2] = ubyte_to_float(upperRightTexel[2], bits[2]);
					texel[3] = 1.0;
				}
				else {
					baseFormat = GL_RG;
					bits[0] = r;
					bits[1] = g;
					bits[2] = 0;
					bits[3] = 0;
					texel[0] = ubyte_to_float(upperRightTexel[0], bits[0]);
					texel[1] = ubyte_to_float(upperRightTexel[1], bits[1]);
					texel[2] = 0.0;
					texel[3] = 1.0;
				}
			}
			else {
				baseFormat = GL_RED;
				bits[0] = r;
				bits[1] = 0;
				bits[2] = 0;
				bits[3] = 0;
				texel[0] = ubyte_to_float(upperRightTexel[0], bits[0]);
				texel[1] = 0.0;
				texel[2] = 0.0;
				texel[3] = 1.0;
			}
		}

		(void) baseFormat;  /* not used, at this time */
	}

	/* compute texel color blended with background color */
#if DO_BLEND
	expected[0] = texel[0] * texel[3] + clearColor[0] * (1.0 - texel[3]);
	expected[1] = texel[1] * texel[3] + clearColor[1] * (1.0 - texel[3]);
	expected[2] = texel[2] * texel[3] + clearColor[2] * (1.0 - texel[3]);
	expected[3] = texel[3] * texel[3] + clearColor[3] * (1.0 - texel[3]);
#else
        expected[0] = texel[0];
        expected[1] = texel[1];
        expected[2] = texel[2];
        expected[3] = texel[3];
#endif

	assert(expected[0] == expected[0]);

	tolerance[0] = bits_to_tolerance(bits[0], compressed);
	tolerance[1] = bits_to_tolerance(bits[1], compressed);
	tolerance[2] = bits_to_tolerance(bits[2], compressed);
	tolerance[3] = bits_to_tolerance(bits[3], compressed);
}


static GLboolean
colors_equal(const GLfloat expected[4], const GLfloat pix[4],
	     GLfloat tolerance[4])
{
	if (fabsf(expected[0] - pix[0]) > tolerance[0] ||
		 fabsf(expected[1] - pix[1]) > tolerance[1] ||
		 fabsf(expected[2] - pix[2]) > tolerance[2] ||
		 fabsf(expected[3] - pix[3]) > tolerance[3]) {
		return GL_FALSE;
	}
	return GL_TRUE;
}


static GLboolean
test_format(const struct test_desc *test,
	    const struct format_desc *fmt)
{
	int x, y;
	int w = TEX_WIDTH, h = TEX_HEIGHT;
	GLfloat readback[TEX_HEIGHT][TEX_WIDTH][4];
	GLubyte upperRightTexel[4];
	int level;
	GLfloat expected[4], pix[4], tolerance[4];
	GLboolean pass = GL_TRUE;

	glClear(GL_COLOR_BUFFER_BIT);

	if (!piglit_automatic) {
		draw_format_text(test, fmt);
	}

	if (fmt->internalformat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT ||
		 fmt->internalformat == GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT) {
		/* The RGBA_DXT1 formats seem to expose a Mesa/libtxc_dxtn bug.
		 * Just skip them for now.  Testing the other compressed formats
		 * is good enough.
		 */
		draw_unsupported();
	}
	else {
		/* init texture image */
		if (!make_texture_image(fmt->internalformat, upperRightTexel))
			return GL_TRUE; /* unsupported = OK */

		x = 10;
		y = 40;

		compute_expected_color(fmt, upperRightTexel, expected, tolerance);

		/* Draw with the texture */
		glEnable(GL_TEXTURE_2D);
#if DO_BLEND
		glEnable(GL_BLEND);
#endif
		piglit_draw_rect_tex(x, y, w, h,  0.0, 0.0, 1.0, 1.0);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);

		if (!piglit_automatic)
			print(x, y - 18, "Textured Quad");

		x += TEX_WIDTH + 20;

		level = 0;
		while (w > 0) {
			/* Get the texture image */
			assert(!glIsEnabled(GL_TEXTURE_2D));
			glGetTexImage(GL_TEXTURE_2D, level, GL_RGBA, GL_FLOAT, readback);

			assert(!glIsEnabled(GL_TEXTURE_2D));
			/* Draw the texture image */
			glWindowPos2iARB(x, y);
#if DO_BLEND
			glEnable(GL_BLEND);
#endif
			assert(!glIsEnabled(GL_TEXTURE_2D));
			glDrawPixels(w, h, GL_RGBA, GL_FLOAT, readback);
			glDisable(GL_BLEND);

			assert(!glIsEnabled(GL_TEXTURE_2D));
			if (!piglit_automatic) {
				if (level == 0) {
					print(x, y - 18, "glGetTexImage");
					print(x, y - 30, "level 0");
				}
				else {
					char s[10];
					sprintf(s, "%d", level);
					print(x, y - 18, s);
				}
			}

			if (level <= 2) {
				GLint rx = x + w-1;
				GLint ry = y + h-1;
				glReadPixels(rx, ry, 1, 1, GL_RGBA, GL_FLOAT, pix);
				if (!colors_equal(expected, pix, tolerance)) {
					printf("%s failure: format: %s, level %d at pixel(%d, %d)\n",
							 TestName, fmt->name, level, rx, ry);
					printf(" Expected (%f, %f, %f, %f)\n",
							 expected[0], expected[1], expected[2], expected[3]);
					printf("	 Found (%f, %f, %f, %f)\n",
							 pix[0], pix[1], pix[2], pix[3]);
					printf("Tolerance (%f, %f, %f, %f)\n",
							 tolerance[0], tolerance[1], tolerance[2], tolerance[3]);
					pass = GL_FALSE;
				}
			}

			x += w + 20;
			w /= 2;
			h /= 2;
			level++;
		}

	}

	piglit_present_results();

	return pass;
}


/**
 * Is the given set of formats supported?
 * This checks if required extensions are present and if this piglit test
 * can actually grok the formats.
 */
static GLboolean
supported_format_set(const struct test_desc *set)
{
	if (!supported(set))
		return GL_FALSE;

	if (set->format == ext_texture_integer ||
		 set->format == ext_packed_depth_stencil ||
		 set->format == arb_texture_rg_int ||
		 set->format == arb_depth_texture ||
		 set->format == arb_depth_buffer_float) {
		/*
		 * texture_integer requires a fragment shader, different
		 * glTexImage calls.  Depth/stencil formats not implemented.
		 */
		return GL_FALSE;
	}

	return GL_TRUE;
}


static GLboolean
test_all_formats(void)
{
	GLboolean pass = GL_TRUE;
	int i, j;

	for (i = 0; i < ARRAY_SIZE(test_sets); i++) {
		const struct test_desc *set = &test_sets[i];
		if (supported_format_set(set)) {
			for (j = 0; j < set->num_formats; j++) {
				if (!test_format(set, &set->format[j])) {
					pass = GL_FALSE;
				}
			}
		}
	}

	return pass;
}


enum piglit_result
piglit_display(void)
{
	GLboolean pass;

	piglit_ortho_projection(piglit_width, piglit_height, GL_FALSE);

	if (piglit_automatic) {
		pass = test_all_formats();
	}
	else {
		const struct test_desc *set = &test_sets[test_index];
		if (supported_format_set(set)) {
			pass = test_format(set, &set->format[format_index]);
		}
		else {
			glClear(GL_COLOR_BUFFER_BIT);
			draw_format_text(set, &set->format[format_index]);
			draw_unsupported();
			piglit_present_results();
		}
	}

	return pass ? PIGLIT_PASS : PIGLIT_FAIL;
}

void
piglit_init(int argc, char **argv)
{
	GLuint t;

	if ((piglit_get_gl_version() < 14) && !piglit_is_extension_supported("GL_ARB_window_pos")) {
		printf("Requires GL 1.4 or GL_ARB_window_pos");
		piglit_report_result(PIGLIT_SKIP);
	}

	fbo_formats_init(argc, argv, 0);
	(void) fbo_formats_display;

	glGenTextures(1, &t);
	glBindTexture(GL_TEXTURE_2D, t);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
}
