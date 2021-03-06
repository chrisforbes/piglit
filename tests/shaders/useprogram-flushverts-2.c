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
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 * Authors:
 *    Eric Anholt <eric@anholt.net>
 *
 */

/** @file useprogram-flushverts-1.c
 *
 * Tests that a change in the shader results in previous vertices
 * getting flushed correctly with the previous shader.
 */

#include "piglit-util-gl-common.h"

PIGLIT_GL_TEST_MAIN(
    100 /*window_width*/,
    100 /*window_height*/,
    GLUT_RGB | GLUT_ALPHA | GLUT_DOUBLE)

static GLint prog1, prog2;

enum piglit_result
piglit_display(void)
{
	GLboolean pass = GL_TRUE;
	float green[4] = {0.0, 1.0, 0.0, 0.0};
	float blue[4] = {0.0, 0.0, 1.0, 0.0};
	float verts[] = {
		-1.0, -1.0,
		-0.5, -1.0,
		-0.5, 1.0,
		-1.0, 1.0,

		-0.5, -1.0,
		 0.0, -1.0,
		 0.0, 1.0,
		-0.5, 1.0,

		0.0, -1.0,
		0.5, -1.0,
		0.5, 1.0,
		0.0, 1.0,

		0.5, -1.0,
		1.0, -1.0,
		1.0, 1.0,
		0.5, 1.0,
	};

	/* Use a static vertex array, because piglit_draw_rect()
	 * triggers flushes due to array state changes.
	 */
	glVertexPointer(2, GL_FLOAT, 0, verts);
	glEnableClientState(GL_VERTEX_ARRAY);

	glUseProgram(prog1);
	glDrawArrays(GL_QUADS, 0, 4);
	glUseProgram(prog2);
	glDrawArrays(GL_QUADS, 4, 4);
	glUseProgram(prog1);
	glDrawArrays(GL_QUADS, 8, 4);
	glUseProgram(prog2);
	glDrawArrays(GL_QUADS, 12, 4);

	glDisableClientState(GL_VERTEX_ARRAY);

	pass &= piglit_probe_pixel_rgba(piglit_width * 1 / 8, piglit_height / 2,
					green);
	pass &= piglit_probe_pixel_rgba(piglit_width * 3 / 8, piglit_height / 2,
					blue);
	pass &= piglit_probe_pixel_rgba(piglit_width * 5 / 8, piglit_height / 2,
					green);
	pass &= piglit_probe_pixel_rgba(piglit_width * 7 / 8, piglit_height / 2,
					blue);

	piglit_present_results();

	return pass ? PIGLIT_PASS : PIGLIT_FAIL;
}

void
piglit_init(int argc, char **argv)
{
	GLint vs, fs1, fs2;
	const char *vs_source =
		"void main()\n"
		"{\n"
		"	gl_Position = gl_Vertex;\n"
		"}\n";
	const char *fs1_source =
		"void main()\n"
		"{\n"
		"	gl_FragColor = vec4(0.0, 1.0, 0.0, 0.0);\n"
		"}\n";
	const char *fs2_source =
		"void main()\n"
		"{\n"
		"	gl_FragColor = vec4(0.0, 0.0, 1.0, 0.0);\n"
		"}\n";

	if (piglit_get_gl_version() < 20) {
		printf("Requires OpenGL 2.0\n");
		piglit_report_result(PIGLIT_SKIP);
	}

	vs = piglit_compile_shader_text(GL_VERTEX_SHADER, vs_source);
	fs1 = piglit_compile_shader_text(GL_FRAGMENT_SHADER, fs1_source);
	fs2 = piglit_compile_shader_text(GL_FRAGMENT_SHADER, fs2_source);
	prog1 = piglit_link_simple_program(vs, fs1);
	prog2 = piglit_link_simple_program(vs, fs2);
}
