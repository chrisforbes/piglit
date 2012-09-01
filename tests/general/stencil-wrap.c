/*
 * (C) Copyright IBM Corporation 2004
 * All Rights Reserved.
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
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.  IN NO EVENT SHALL
 * VA LINUX SYSTEM, IBM AND/OR THEIR SUPPLIERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/**
 * \file stencil_wrap.c
 *
 * Simple test of GL_EXT_stencil_wrap functionality.  Four squares are drawn
 * with different stencil modes, but all should be rendered with the same
 * final color.
 *
 * \author Ian Romanick <idr@us.ibm.com>
 */

#include "piglit-util-gl-common.h"

PIGLIT_GL_TEST_MAIN(
    550 /*window_width*/,
    200 /*window_height*/,
    GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL)

enum piglit_result
piglit_display(void)
{
	GLint  max_stencil;
	GLint  stencil_bits;
	unsigned i;
	GLboolean pass = GL_TRUE;
	float expected[4] = {0.5, 0.5, 0.5, 0.5};
	int w = piglit_width / (5 * 2 + 1);
	int h = w;
	int start_y = (piglit_height - h) / 2;

	piglit_ortho_projection(piglit_width, piglit_height, GL_FALSE);

	glGetIntegerv(GL_STENCIL_BITS, & stencil_bits);
	max_stencil = (1U << stencil_bits) - 1;
	printf("Stencil bits = %u, maximum stencil value = 0x%08x\n",
		stencil_bits, max_stencil);

	glClearStencil(0);
	glClearColor(0.2, 0.2, 0.8, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
		GL_STENCIL_BUFFER_BIT);

	/* This is the "reference" square.
	 */

	glDisable(GL_STENCIL_TEST);
	glColor3f(0.5, 0.5, 0.5);
	piglit_draw_rect(w * 1, start_y, w, h);

	glEnable(GL_STENCIL_TEST);

	/* Draw the first two squares using the two non-wrap (i.e., saturate)
	 * modes.
	 */

	glStencilFunc(GL_ALWAYS, 0, ~0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);

	glColor3f(0.9, 0.9, 0.9);

	for (i = 0 ; i < (max_stencil + 5) ; i++) {
		piglit_draw_rect(w * 3, start_y, w, h);
	}

	glStencilFunc(GL_EQUAL, max_stencil, ~0);
	glColor3f(0.5, 0.5, 0.5);
	piglit_draw_rect(w * 3, start_y, w, h);

	glStencilFunc(GL_ALWAYS, 0, ~0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);

	glColor3f(0.9, 0.9, 0.9);

	for (i = 0 ; i < (max_stencil + 5) ; i++) {
		piglit_draw_rect(w * 5, start_y, w, h);
	}

	glStencilFunc(GL_EQUAL, 0, ~0);
	glColor3f(0.5, 0.5, 0.5);
	piglit_draw_rect(w * 5, start_y, w, h);

	/* Draw the last two squares using the two wrap modes.
	 */

	glStencilFunc(GL_ALWAYS, 0, ~0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR_WRAP);

	glColor3f(0.9, 0.9, 0.9);

	for (i = 0 ; i < (max_stencil + 5) ; i++) {
		piglit_draw_rect(w * 7, start_y, w, h);
	}

	glStencilFunc(GL_EQUAL, 4, ~0);
	glColor3f(0.5, 0.5, 0.5);
	piglit_draw_rect(w * 7, start_y, w, h);

	glStencilFunc(GL_ALWAYS, 0, ~0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_DECR_WRAP);

	glColor3f(0.9, 0.9, 0.9);

	for (i = 0 ; i < 5 ; i++) {
		piglit_draw_rect(w * 9, start_y, w, h);
	}

	glStencilFunc(GL_EQUAL, (max_stencil - 4), ~0);
	glColor3f(0.5, 0.5, 0.5);
	piglit_draw_rect(w * 9, start_y, w, h);

	pass = piglit_probe_pixel_rgb(w * 1.5, piglit_height / 2, expected);
	pass = piglit_probe_pixel_rgb(w * 3.5, piglit_height / 2, expected);
	pass = piglit_probe_pixel_rgb(w * 5.5, piglit_height / 2, expected);
	pass = piglit_probe_pixel_rgb(w * 7.5, piglit_height / 2, expected);
	pass = piglit_probe_pixel_rgb(w * 9.5, piglit_height / 2, expected);

	piglit_present_results();

	return pass ? PIGLIT_PASS : PIGLIT_FAIL;
}

void
piglit_init(int argc, char **argv)
{
	if (!piglit_is_extension_supported("GL_EXT_stencil_wrap") && piglit_get_gl_version() < 14) {
		printf("Sorry, this program requires either "
		       "GL_EXT_stencil_wrap or OpenGL 1.4.\n");
		piglit_report_result(PIGLIT_SKIP);
	}

	printf("\nAll 5 squares should be the same color.\n");
	glEnable(GL_BLEND);
}
