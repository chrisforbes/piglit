/*
 * Copyright (c) The Piglit project 2007
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
 * Test whether LIT honours the output mask.
 */

#include "piglit-util-gl-common.h"

PIGLIT_GL_TEST_MAIN(
    200 /*window_width*/,
    200 /*window_height*/,
    GLUT_RGB | GLUT_ALPHA | GLUT_DOUBLE | GLUT_DEPTH)

static GLuint FragProg[15];

static const char fragProgramTemplate[] =
	"!!ARBfp1.0\n"
	"PARAM values = { 0.65, 0.9, 0.0, 8.0 }; \n"
	"PARAM bogus = { 0.8, 0.8, 0.8, 0.8 }; \n"
	"MOV result.color, bogus; \n"
	"LIT result.color.%s, values; \n"
	"END\n";
static float LitExpected[4] = { 1.0, 0.65, 0.433, 1.0 };

static void DoFrame(void)
{
	int mask;

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glEnable(GL_FRAGMENT_PROGRAM_ARB);

	for(mask = 1; mask < 16; ++mask) {
		glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, FragProg[mask-1]);
		glPushMatrix();
		glTranslatef((mask % 4), (mask / 4), 0.0);

		glBegin(GL_QUADS);
			glVertex2f(0, 0);
			glVertex2f(1, 0);
			glVertex2f(1, 1);
			glVertex2f(0, 1);
		glEnd();

		glPopMatrix();
	}
}

static int DoTest( void )
{
	int mask;
	GLfloat dmax;

	dmax = 0;

	for(mask = 1; mask < 16; ++mask) {
		GLfloat probe[4];
		GLfloat delta[4];
		int i;

		glReadPixels(piglit_width * (2*(mask%4)+1)/8,
			     piglit_height * (2*(mask/4)+1)/8, 1, 1,
				GL_RGBA, GL_FLOAT, probe);

		printf("Probe %i: %f,%f,%f,%f\n", mask, probe[0], probe[1], probe[2], probe[3]);

		for(i = 0; i < 4; ++i) {
			if (mask & (1 << i))
				delta[i] = probe[i] - LitExpected[i];
			else
				delta[i] = probe[i] - 0.8;

			if (delta[i] > dmax) dmax = delta[i];
			else if (-delta[i] > dmax) dmax = -delta[i];
		}

		printf("   Delta: %f,%f,%f,%f\n", delta[0], delta[1], delta[2], delta[3]);
	}

	printf("Max delta: %f\n", dmax);

	if (dmax >= 0.02)
		return 0;
	else
		return 1;
}

enum piglit_result
piglit_display(void)
{
	int pass;

	DoFrame();
	pass = DoTest();
	piglit_present_results();

	return pass ? PIGLIT_PASS : PIGLIT_FAIL;
}

void piglit_init(int argc, char **argv)
{
	int mask;

	printf("GL_RENDERER = %s\n", (char *) glGetString(GL_RENDERER));

	piglit_require_fragment_program();

	/*
	 * Fragment programs
	 */
	for(mask = 1; mask < 16; ++mask) {
		char programText[1024];
		char maskstring[5];

		maskstring[0] = 0;
		if (mask & 1) strcat(maskstring, "x");
		if (mask & 2) strcat(maskstring, "y");
		if (mask & 4) strcat(maskstring, "z");
		if (mask & 8) strcat(maskstring, "w");
		sprintf(programText, fragProgramTemplate, maskstring);

		FragProg[mask-1] = piglit_compile_program(GL_FRAGMENT_PROGRAM_ARB, programText);
	}

	piglit_ortho_projection(4.0, 4.0, GL_FALSE);
}
