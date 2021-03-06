/*
 * Copyright © 2009 Intel Corporation
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
 *
 * Authors:
 *    Ben Holmes <shranzel@hotmail.com>
 *    Marek Olšák <maraeo@gmail.com>
 */

#include "piglit-util-gl-common.h"

PIGLIT_GL_TEST_MAIN(
    400 /*window_width*/,
    300 /*window_height*/,
    GLUT_RGB | GLUT_DOUBLE)

static GLuint vbo;

void
piglit_init(int argc, char **argv)
{
	piglit_ortho_projection(piglit_width, piglit_height, GL_FALSE);

	piglit_require_extension("GL_ARB_vertex_buffer_object");

	glGenBuffersARB(1, &vbo);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, 12 * sizeof(GLfloat),
			NULL, GL_DYNAMIC_DRAW);
}

enum piglit_result
piglit_display(void)
{
	GLfloat white[4] = {1.0, 1.0, 1.0, 0.0};
	GLboolean pass = GL_TRUE;
	GLfloat varray1[12] = {175, 125, 0,
			       175, 175, 0,
			       125, 125, 0,
			       125, 175, 0};
	GLfloat varray2[12] = {275, 125, 0,
			       275, 175, 0,
			       225, 125, 0,
			       225, 175, 0};
	GLenum err;

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	glBufferDataARB(GL_ARRAY_BUFFER_ARB, 12 * sizeof(GLfloat),
			varray1, GL_DYNAMIC_DRAW);

	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBufferDataARB(GL_ARRAY_BUFFER_ARB, 12 * sizeof(GLfloat),
			varray2, GL_DYNAMIC_DRAW);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	if ((err = glGetError()) != 0) {
		printf("gl error: 0x%08x\n", err);
		pass = GL_FALSE;
	}

	pass = pass && piglit_probe_pixel_rgb(250, 150, white);
	pass = pass && piglit_probe_pixel_rgb(150, 150, white);

	piglit_present_results();

	glDisableClientState(GL_VERTEX_ARRAY);

	return pass ? PIGLIT_PASS : PIGLIT_FAIL;
}
