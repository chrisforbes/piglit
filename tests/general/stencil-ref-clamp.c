/*
 * Copyright © 2013 Chris Forbes <chrisf@ijw.co.nz>
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
 * Test for correct stencil reference value clamping.
 *
 * From the GL 4.3 spec, 17.3.35 "Stencil Test":
 *
 *      "Stencil comparison operations and queries of <ref> clamp its value
 *      to the range [0, 2^s − 1], where <s> is the number of bits in the
 *      stencil buffer attached to the draw framebuffer."
 *
 * Until recently, Mesa clamped the value at specification time instead.
 */

#include "piglit-util-gl-common.h"

PIGLIT_GL_TEST_CONFIG_BEGIN

config.supports_gl_compat_version = 20;
config.window_visual = PIGLIT_GL_VISUAL_DOUBLE | PIGLIT_GL_VISUAL_RGBA;

PIGLIT_GL_TEST_CONFIG_END

void
piglit_init(int argc, char **argv)
{
	GLuint fbo;
	GLuint rb;
	GLint stencil_bits = 1;
	GLint stencil_ref;
	GLint expected_stencil_ref;

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	/* ensure we have no stencil bits */
	glGetIntegerv(GL_STENCIL_BITS, &stencil_bits);
	if (stencil_bits != 0) {
		printf("initial stencil bits: expected 0, got %d\n",
			stencil_bits);
		piglit_report_result(PIGLIT_SKIP);
	}

	/* set the reference value. this value requires more than 8 bits
	 * so will be clamped with S8 bound.
	 */
	glStencilFuncSeparate(GL_FRONT_AND_BACK, GL_EQUAL, 0x200, 0xff);

	/* read back the stencil ref; ensure it is clamped */
	glGetIntegerv(GL_STENCIL_REF, &stencil_ref);
	if (stencil_ref != 0) {
		printf("stencil ref with no stencil attachment:"
			" expected 0, got 0x%x\n", stencil_ref);
		piglit_report_result(PIGLIT_FAIL);
	}

	/* now add a stencil attachment */
	glGenRenderbuffers(1, &rb);
	glBindRenderbuffer(GL_RENDERBUFFER, rb);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, 64, 64);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
		GL_RENDERBUFFER, rb);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
		GL_FRAMEBUFFER_COMPLETE) {
		printf("framebuffer incomplete, not going to work.\n");
		piglit_report_result(PIGLIT_SKIP);
	}

	/* now read back the stencil depth, should be 8. */
	glGetIntegerv(GL_STENCIL_BITS, &stencil_bits);
	if (stencil_bits == 0) {
		printf("stencil depth with S8 renderbuffer bound: expected"
			" nonzero, got %d\n", stencil_bits);
		piglit_report_result(PIGLIT_FAIL);
	}

	expected_stencil_ref = (1<<stencil_bits) - 1;

	/* finally read back the stencil ref; should match expected. */
	glGetIntegerv(GL_STENCIL_REF, &stencil_ref);
	if (stencil_ref != expected_stencil_ref) {
		printf("stencil ref with stencil buffer bound:"
			" expected 0x%x, got 0x%x\n",
			expected_stencil_ref, stencil_ref);
		piglit_report_result(PIGLIT_FAIL);
	}

	piglit_report_result(PIGLIT_PASS);

}

enum piglit_result
piglit_display(void)
{
	return PIGLIT_FAIL;     /* shouldn't get here */
}
