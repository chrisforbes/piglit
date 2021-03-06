/*
 * Copyright © 2010 Intel Corporation
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
 * \file link-mismatch-layout-03.c
 * Verify that linking passes when layout qualifiers are used
 *
 * Attmpt to link two fragment shaders.  One shader specifies
 * \c pixel_center_interger layout of \c gl_FragCoord, and the other does not
 * specify a layout qualifier for \c gl_FragCoord.  Linking should succeed.
 *
 * \author Ian Romanick <ian.d.romanick@intel.com>
 */
#include "piglit-util-gl-common.h"

PIGLIT_GL_TEST_MAIN(
    100 /*window_width*/,
    100 /*window_height*/,
    GLUT_RGB | GLUT_DOUBLE)

static const char layout_center[] =
	"#extension GL_ARB_fragment_coord_conventions: enable\n"
	"layout(pixel_center_integer) varying vec4 gl_FragCoord;\n"
	"vec4 foo(void);\n"
	"void main(void)\n"
	"{\n"
	"    gl_FragColor = foo();\n"
	"}\n";

static const char layout_upper[] =
	"vec4 foo(void)\n"
	"{\n"
	"    return vec4(1.0);\n"
	"}\n";

enum piglit_result
piglit_display(void)
{
	return PIGLIT_FAIL;
}

void
piglit_init(int argc, char **argv)
{
	GLint ok;
	GLuint prog;
	GLuint fs[2];

	if (piglit_get_gl_version() < 20) {
		printf("Requires OpenGL 2.0\n");
		piglit_report_result(PIGLIT_SKIP);
	}

	piglit_require_extension("GL_ARB_fragment_coord_conventions");

	fs[0] = piglit_compile_shader_text(GL_FRAGMENT_SHADER, layout_center);
	fs[1] = piglit_compile_shader_text(GL_FRAGMENT_SHADER, layout_upper);
	prog = glCreateProgram();
	glAttachShader(prog, fs[0]);
	glAttachShader(prog, fs[1]);
	glLinkProgram(prog);
	glDeleteShader(fs[0]);
	glDeleteShader(fs[1]);

	ok = piglit_link_check_status(prog);
	if (!ok) {
		fprintf(stderr,
			"Linking with gl_FragCoord layouts "
			"failed when it should have succeeded.\n");
		piglit_report_result(PIGLIT_FAIL);
	}

	piglit_report_result(PIGLIT_PASS);
}
