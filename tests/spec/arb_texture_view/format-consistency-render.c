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
 *
 * Author: Chris Forbes <chrisf@ijw.co.nz>
 */

/**
 * Tests format consistency for texture views, across all formats in each view class.
 * Based on the OpenGL 4.4 spec, section 8.26 "Texture Image Loads and Stores".
 *
 * Hardware will typically implement views by arranging for the memory layouts to
 * be trivially aliasable, but the spec is written in terms of conversions via
 * scratch memory.
 *
 * This test ensures that whatever the hardware is doing is consistent with the
 * specified conversions.
 */

#include "piglit-util-gl-common.h"
#include "common.h"
#include "view-classes.h"

PIGLIT_GL_TEST_CONFIG_BEGIN

	config.supports_gl_compat_version = 30;

	config.window_visual = PIGLIT_GL_VISUAL_RGBA | PIGLIT_GL_VISUAL_DOUBLE;

PIGLIT_GL_TEST_CONFIG_END

bool
do_test(int bits, struct format_info *fmt)
{
	GLuint tex;
	bool pass = true;

	/* reference pixel data -- up to 16 bytes */
	char ref[] = {0x0f, 0x1e, 0x2d, 0x3c, 0x4b, 0x5a, 0x69, 0x78,
		      0x87, 0x96, 0xa5, 0xb4, 0xc3, 0xd2, 0xe1, 0xf0};
	float green[] = {0,1,0,0};

	printf("Testing %d bits class:\n", bits);

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexStorage2D(GL_TEXTURE_2D, 1, fmt->internalformat,
		       1, 1);

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1, 1,
			fmt->pixelformat, fmt->pixeltype,
			ref);

	for (; fmt->internalformat; fmt++) {
		GLuint view, compare, prog;
		char *fs;

		glGenTextures(1, &view);
		glTextureView(view, GL_TEXTURE_2D, tex,
			      fmt->internalformat,
			      0, 1, 0, 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, view);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glGenTextures(1, &compare);
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, compare);
		glTexImage2D(GL_TEXTURE_2D, 0, fmt->internalformat,
			     1, 1, 0, fmt->pixelformat, fmt->pixeltype,
			     ref);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		asprintf(&fs,
			"#version 130\n"
			"uniform %ssampler2D test, ref;\n"
			"void main() {\n"
			"	vec2 tc = vec2(0,0);\n"
			"	if (texture(test, tc) != texture(ref, tc)) {\n"
			"		gl_FragColor = vec4(1,0,0,0);\n"
			"	} else {\n"
			"		gl_FragColor = vec4(0,1,0,0);\n"
			"	}\n"
			"}\n", fmt->sampler_prefix);
		prog = piglit_build_simple_program(
			"#version 130\n"
			"void main() {\n"
			"	gl_Position = gl_Vertex;\n"
			"}\n",
			fs);
		free(fs);

		glUseProgram(prog);
		glUniform1i(glGetUniformLocation(prog, "test"), 0);
		glUniform1i(glGetUniformLocation(prog, "ref"), 1);

		glClearColor(0.2, 0.2, 0.2, 0.2);
		glClear(GL_COLOR_BUFFER_BIT);
		glViewport(0, 0, 64, 64);
		piglit_draw_rect(-1, -1, 2, 2);

		if (!piglit_probe_pixel_rgba(32, 32, green)) {
			pass = false;
			piglit_report_subtest_result(PIGLIT_FAIL,
				piglit_get_gl_enum_name(fmt->internalformat));
		}
		else {
			piglit_report_subtest_result(PIGLIT_PASS,
				piglit_get_gl_enum_name(fmt->internalformat));
		}

		piglit_present_results();

		glUseProgram(0);
		glDeleteProgram(prog);
		glDeleteTextures(1, &compare);
		glDeleteTextures(1, &view);
	}

	glDeleteTextures(1, &tex);
	return pass;
}

enum piglit_result
piglit_display(void)
{
	bool pass = true;
	pass = do_test(8, view_class_8bits) && pass;
	pass = do_test(16, view_class_16bits) && pass;
	pass = do_test(24, view_class_24bits) && pass;
	pass = do_test(32, view_class_32bits) && pass;
	pass = do_test(48, view_class_48bits) && pass;
	pass = do_test(64, view_class_64bits) && pass;
	pass = do_test(96, view_class_96bits) && pass;
	pass = do_test(128, view_class_128bits) && pass;

	return pass ? PIGLIT_PASS : PIGLIT_FAIL;
}

void
piglit_init(int argc, char **argv)
{
	piglit_require_extension("GL_ARB_texture_view");
}
