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
 *  \file
 *  This tests that rendering into a view works when the view has a different
 *  internalformat to the original texture.
 */

#include "piglit-util-gl-common.h"
#include "common.h"

PIGLIT_GL_TEST_CONFIG_BEGIN

	config.supports_gl_compat_version = 30;
	config.window_visual = PIGLIT_GL_VISUAL_RGBA | PIGLIT_GL_VISUAL_DOUBLE;

PIGLIT_GL_TEST_CONFIG_END

#define TEX_SIZE 64

const float expected[] = {1.0f, 0.25f, -1.0f, 0.0f};
GLuint fbo, tex;

enum piglit_result
piglit_display(void)
{
	bool pass = true;
	glViewport(0, 0, TEX_SIZE, TEX_SIZE);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	piglit_draw_rect(-1, -1, 2, 2);

	glBindTexture(GL_TEXTURE_2D, tex);

	pass = piglit_probe_texel_rect_rgba(GL_TEXTURE_2D, 0,
		0, 0, TEX_SIZE, TEX_SIZE, expected) && pass;

	return pass ? PIGLIT_PASS : PIGLIT_FAIL;
}


void
piglit_init(int argc, char **argv)
{
	GLuint view, prog;

	piglit_require_extension("GL_ARB_texture_storage");
	piglit_require_extension("GL_ARB_texture_view");

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F,
		TEX_SIZE, TEX_SIZE);

	/* make an unsigned integer view */
	glGenTextures(1, &view);
	glTextureView(view, GL_TEXTURE_2D, tex, GL_RGBA32UI,
			0, 1, 0, 1);
	glBindTexture(GL_TEXTURE_2D, view);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	/* shader to render uint texture */
	prog = piglit_build_simple_program(
			"#version 130\n"
			"void main() { \n"
			"	gl_Position = gl_Vertex;\n"
			"}\n",

			"#version 130\n"
			"out uvec4 result;\n"
			"void main() { \n"
			"	result = uvec4(\n"
			"		0x3f800000u,\n"
			"		0x3e800000u,\n"
			"		0xbf800000u,\n"
			"		0x00000000u);\n"
			"}\n");

	glBindFragDataLocation(prog, 0, "result");
	glUseProgram(prog);

	/* fbo with uint texture as color buffer */
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, view, 0);
}
