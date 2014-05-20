/*
 * Copyright © 2014 Intel Corporation
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
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

/**
 * \file api.c
 *
 * Test API behavior for the ARB_compressed_texture_pixel_storage spec.
 *
 */
#include "piglit-util-gl-common.h"

PIGLIT_GL_TEST_CONFIG_BEGIN

	config.supports_gl_compat_version = 12;

	config.window_visual = PIGLIT_GL_VISUAL_RGBA;

PIGLIT_GL_TEST_CONFIG_END

enum piglit_result
piglit_display(void)
{
	return PIGLIT_FAIL;
}


GLuint tex2d, tex2d_array;
unsigned char pixel_data[16] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};	/* two blocks */


static bool
test_enum(GLenum pname, GLint expected_old, GLint value)
{
	GLint old, new;
	glGetIntegerv(pname, &old);

	if (!piglit_check_gl_error(GL_NO_ERROR))
		return false;

	if (old != expected_old)
		return false;

	glPixelStorei(pname, value);

	if (!piglit_check_gl_error(GL_NO_ERROR))
		return false;

	glGetIntegerv(pname, &new);

	if (!piglit_check_gl_error(GL_NO_ERROR))
		return false;

	if (new != value)
		return false;

	return true;
}


#define X(name, expr) \
	do { \
		printf("Testing %s\n", name); \
		if (expr) \
			piglit_report_subtest_result(PIGLIT_PASS, name); \
		else { \
			piglit_report_subtest_result(PIGLIT_FAIL, name); \
			pass = false; \
		} \
	} while (0)


static bool
test_state(void)
{
	bool pass = true;

	glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);

	X("set-unpack-block-width",
		test_enum(GL_UNPACK_COMPRESSED_BLOCK_WIDTH, 0, 4));
	X("set-unpack-block-height",
		test_enum(GL_UNPACK_COMPRESSED_BLOCK_HEIGHT, 0, 4));
	X("set-unpack-block-depth",
		test_enum(GL_UNPACK_COMPRESSED_BLOCK_DEPTH, 0, 1));
	X("set-unpack-block-size",
		test_enum(GL_UNPACK_COMPRESSED_BLOCK_SIZE, 0, 8));

	X("set-pack-block-width",
		test_enum(GL_PACK_COMPRESSED_BLOCK_WIDTH, 0, 4));
	X("set-pack-block-height",
		test_enum(GL_PACK_COMPRESSED_BLOCK_HEIGHT, 0, 4));
	X("set-pack-block-depth",
		test_enum(GL_PACK_COMPRESSED_BLOCK_DEPTH, 0, 1));
	X("set-pack-block-size",
		test_enum(GL_PACK_COMPRESSED_BLOCK_SIZE, 0, 8));

	glPopClientAttrib();

	return pass;
}


static void
load_one_block_2d(unsigned char *data)
{
	glCompressedTexImage2D(GL_TEXTURE_2D, 0,
			       GL_COMPRESSED_SIGNED_RED_RGTC1_EXT,
			       4, 4, 0, 8,
			       data);
}


static void
load_one_block_3d(unsigned char *data)
{
	glCompressedTexImage3D(GL_TEXTURE_2D_ARRAY, 0,
			       GL_COMPRESSED_SIGNED_RED_RGTC1_EXT,
			       4, 4, 1, 0, 8,
			       data);
}


static bool
verify_image(GLenum target, unsigned char *expected)
{
	int i;
	unsigned char actual[8];
	glGetCompressedTexImage(target, 0, actual);

	if (!memcmp(expected, actual, sizeof(actual)))
		return true;

	printf("Image mismatch: Expected:\n  ");
	for (i = 0; i < sizeof(actual); i++)
		printf("%02x ", expected[i]);
	printf("\nActual:\n  ");
	for (i = 0; i < sizeof(actual); i++)
		printf("%02x ", actual[i]);
	printf("\n");

	return false;
}


static bool
test_unpack_skippixels(void)
{
	bool pass = true;

	glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);

	glPixelStorei(GL_UNPACK_COMPRESSED_BLOCK_SIZE, 8);
	glPixelStorei(GL_UNPACK_COMPRESSED_BLOCK_WIDTH, 4);

	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 2);	/* not a multiple of bw */
	load_one_block_2d(pixel_data);
	X("unpack-skip-pixels-bad", piglit_check_gl_error(GL_INVALID_OPERATION));

	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 4);	/* multiple of bw */
	load_one_block_2d(pixel_data);
	X("unpack-skip-pixels-ok", piglit_check_gl_error(GL_NO_ERROR));

	X("unpack-skip-pixels-readback", verify_image(GL_TEXTURE_2D, pixel_data + 8));

	glPopClientAttrib();

	return pass;
}


static bool
test_unpack_skiprows(void)
{
	bool pass = true;

	glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);

	glPixelStorei(GL_UNPACK_COMPRESSED_BLOCK_SIZE, 8);
	glPixelStorei(GL_UNPACK_COMPRESSED_BLOCK_HEIGHT, 4);

	glPixelStorei(GL_UNPACK_SKIP_ROWS, 2);	/* not a multiple of bh */
	load_one_block_2d(pixel_data);
	X("unpack-skip-rows-bad", piglit_check_gl_error(GL_INVALID_OPERATION));

	glPixelStorei(GL_UNPACK_SKIP_ROWS, 4);	/* multiple of bh */
	load_one_block_2d(pixel_data);
	X("unpack-skip-rows-ok", piglit_check_gl_error(GL_NO_ERROR));

	X("unpack-skip-rows-readback", verify_image(GL_TEXTURE_2D, pixel_data + 8));

	glPopClientAttrib();

	return pass;
}


static bool
test_unpack_skipimages(void)
{
	bool pass = true;

	glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);

	glPixelStorei(GL_UNPACK_COMPRESSED_BLOCK_SIZE, 8);
	/* set this bogusly just so we can hit this case: in practice, this is
	 * always 1 for every format. */
	glPixelStorei(GL_UNPACK_COMPRESSED_BLOCK_DEPTH, 4);

	glPixelStorei(GL_UNPACK_SKIP_IMAGES, 2);	/* not a multiple of bd */
	load_one_block_3d(pixel_data);
	X("unpack-skip-images-bad", piglit_check_gl_error(GL_INVALID_OPERATION));

	glPixelStorei(GL_UNPACK_COMPRESSED_BLOCK_DEPTH, 1);	/* the real thing. */
	glPixelStorei(GL_UNPACK_SKIP_IMAGES, 1);	/* multiple of bd */
	load_one_block_3d(pixel_data);
	X("unpack-skip-images-ok", piglit_check_gl_error(GL_NO_ERROR));

	X("unpack-skip-images-readback", verify_image(GL_TEXTURE_2D_ARRAY, pixel_data + 8));

	glPopClientAttrib();

	return pass;
}


void
piglit_init(int argc, char **argv)
{
	bool pass = true;
	piglit_require_extension("GL_ARB_compressed_texture_pixel_storage");
	/* Doesn't really matter which compressed format we choose, but this one
	 * is pretty straightforward to understand.
	 */
	piglit_require_extension("GL_EXT_texture_compression_rgtc");

	glGenTextures(1, &tex2d);
	glGenTextures(1, &tex2d_array);
	glBindTexture(GL_TEXTURE_2D, tex2d);
	glBindTexture(GL_TEXTURE_2D_ARRAY, tex2d_array);

	pass = test_state() && pass;
	pass = test_unpack_skippixels() && pass;
	pass = test_unpack_skiprows() && pass;
	pass = test_unpack_skipimages() && pass;

	piglit_report_result(pass ? PIGLIT_PASS : PIGLIT_FAIL);
}
