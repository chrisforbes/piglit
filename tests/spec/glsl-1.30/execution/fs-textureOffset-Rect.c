/*
 * Copyright 2013 Intel Corporation
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
 * \file fs-textureOffset-Rect.c
 *
 * Tests the built-in function textureOffset(sampler2DRect, ..) in the fragment shader.
 */
#include "piglit-util-gl-common.h"

PIGLIT_GL_TEST_CONFIG_BEGIN

	config.supports_gl_compat_version = 30;

	config.window_visual = PIGLIT_GL_VISUAL_RGB | PIGLIT_GL_VISUAL_DOUBLE;

PIGLIT_GL_TEST_CONFIG_END

const int tex_size = 64;

static const char vs[] =
"#version 130\n"
"void main()\n"
"{\n"
"	gl_Position = gl_Vertex;\n"
"}\n";

static const char fs[] =
"#version 130\n"
"uniform sampler2DRect tex;\n"
"void main()\n"
"{\n"
"       const ivec2 offset = ivec2(-8, 7);\n"
"       float texel = textureOffset(tex, gl_FragCoord.xy, offset).x;\n"
"	vec2 coord = (1/64.0) * clamp(gl_FragCoord.xy + offset - vec2(0.5), vec2(0), vec2(63));\n"
"	float expected = fract(1 + coord.x - coord.y); \n"
"	gl_FragColor = vec4(texel - expected, 1 - (expected - texel), 0, 1);\n"
"}\n";

static float green[] = { 0, 1, 0 };

enum piglit_result
piglit_display(void)
{
	bool pass = true;

	glClearColor(0.5, 0.5, 0.5, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glViewport(0, 0, tex_size, tex_size);
	piglit_draw_rect(-1, -1, 2, 2);
	pass = piglit_probe_rect_rgb(0, 0, tex_size, tex_size, green);

	piglit_present_results();

	return pass ? PIGLIT_PASS : PIGLIT_FAIL;
}

void
piglit_init(int argc, char **argv)
{
	int prog;
	GLuint tex;
	unsigned char tex_data[tex_size * tex_size];
	int i, j;

	for (j = 0; j < tex_size; j++) {
		for (i = 0; i < tex_size; i++) {
			tex_data[j * tex_size + i] = 4 * i - 4 * j;
		}
	}

	piglit_require_GLSL_version(130);

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_RECTANGLE, tex);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RED,
		     tex_size, tex_size, 0, GL_RED, GL_UNSIGNED_BYTE, tex_data);

	prog = piglit_build_simple_program(vs, fs);

	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glUseProgram(prog);
}
