/*
 * Copyright © 2013 Intel Corporation
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
 * \file api-errors.c
 *
 * This tests a few error conditions from the ARB_vertex_type_10f_11f_11f_rev extension.
 */

#include "piglit-util-gl-common.h"

PIGLIT_GL_TEST_CONFIG_BEGIN
	config.supports_gl_compat_version = 20;
	config.window_visual = PIGLIT_GL_VISUAL_RGB | PIGLIT_GL_VISUAL_DOUBLE;
PIGLIT_GL_TEST_CONFIG_END


enum piglit_result
piglit_display()
{
	return PIGLIT_FAIL;	/* unreached */
}

#define TEST(name, err) \
	do { \
		if (!piglit_check_gl_error(err)) { \
			piglit_report_subtest_result(PIGLIT_FAIL, name); \
			pass = false; \
		} \
		else { \
			piglit_report_subtest_result(PIGLIT_PASS, name); \
		} \
	} while(0);


static bool
test_vertex_attrib_format()
{
	bool pass = true;

	/* "The error INVALID_OPERATION is generated by VertexAttribFormat, ...
	 * if <type> is UNSIGNED_INT_10F_11F_11F_REV and <size> is not 3."
	 */

	glVertexAttribFormat(0, 3, GL_UNSIGNED_INT_10F_11F_11F_REV, GL_FALSE, 0);
	TEST("VertexAttribFormat-ok", GL_NO_ERROR);

	glVertexAttribFormat(0, 2, GL_UNSIGNED_INT_10F_11F_11F_REV, GL_FALSE, 0);
	TEST("VertexAttribFormat-badsize", GL_INVALID_OPERATION);

	glVertexAttribFormat(0, GL_BGRA, GL_UNSIGNED_INT_10F_11F_11F_REV, GL_FALSE, 0);
	TEST("VertexAttribFormat-badsize-bgra", GL_INVALID_OPERATION);

	/* "The error INVALID_ENUM is generated by VertexAttribIFormat,
	 * VertexAttribLFormat, ...
	 * if <type> is UNSIGNED_INT_10F_11F_11F_REV."
	 */

	glVertexAttribIFormat(0, 3, GL_UNSIGNED_INT_10F_11F_11F_REV, 0);
	TEST("VertexAttribIFormat-not-allowed", GL_INVALID_ENUM);

	glVertexAttribLFormat(0, 3, GL_UNSIGNED_INT_10F_11F_11F_REV, 0);
	TEST("VertexAttribLFormat-not-allowed", GL_INVALID_ENUM);

	return pass;
}


static bool
test_legacy_vertex_attribs()
{
	bool pass = true;

	/* "The error INVALID_ENUM is generated by VertexPointer, NormalPointer,
	 *  ColorPointer, SecondaryColorPointer, IndexPointer, EdgeFlagPointer,
	 *  FogCoordPointer, or TexCoordPointer if <type> is
	 *  UNSIGNED_INT_10F_11F_11F_REV.
	 */

	glVertexPointer(3, GL_UNSIGNED_INT_10F_11F_11F_REV, 0, (GLvoid *)0);
	TEST("VertexPointer-not-allowed", GL_INVALID_ENUM);

	glNormalPointer(GL_UNSIGNED_INT_10F_11F_11F_REV, 0, (GLvoid *)0);
	TEST("NormalPointer-not-allowed", GL_INVALID_ENUM);

	glColorPointer(3, GL_UNSIGNED_INT_10F_11F_11F_REV, 0, (GLvoid *)0);
	TEST("ColorPointer-not-allowed", GL_INVALID_ENUM);

	glSecondaryColorPointer(3, GL_UNSIGNED_INT_10F_11F_11F_REV, 0, (GLvoid *)0);
	TEST("SecondaryColorPointer-not-allowed", GL_INVALID_ENUM);

	glIndexPointer(GL_UNSIGNED_INT_10F_11F_11F_REV, 0, (GLvoid *)0);
	TEST("IndexPointer-not-allowed", GL_INVALID_ENUM);

	/* spec lists EdgeFlagPointer, but this is a bug -- it does not take a <type> param */

	glFogCoordPointer(GL_UNSIGNED_INT_10F_11F_11F_REV, 0, (GLvoid *)0);
	TEST("FogCoordPointer-not-allowed", GL_INVALID_ENUM);

	glTexCoordPointer(3, GL_UNSIGNED_INT_10F_11F_11F_REV, 0, (GLvoid *)0);
	TEST("TexCoordPointer-not-allowed", GL_INVALID_ENUM);

	return pass;
}


static bool
test_vertex_attribs()
{
	bool pass = true;
	GLuint data = 0;

	/* "The error INVALID_OPERATION is generated by ... , or VertexAttribPointer
	 * if <type> is UNSIGNED_INT_10F_11F_11F_REV and <size> is not 3.
	 */

	glVertexAttribPointer(0, 3, GL_UNSIGNED_INT_10F_11F_11F_REV, GL_FALSE, 0, (GLvoid *)0);
	TEST("VertexAttribPointer-ok", GL_NO_ERROR);

	glVertexAttribPointer(0, 2, GL_UNSIGNED_INT_10F_11F_11F_REV, GL_FALSE, 0, (GLvoid *)0);
	TEST("VertexAttribPointer-badsize", GL_INVALID_OPERATION);

	glVertexAttribPointer(0, GL_BGRA, GL_UNSIGNED_INT_10F_11F_11F_REV, GL_FALSE, 0, (GLvoid *)0);
	TEST("VertexAttribPointer-badsize-bgra", GL_INVALID_OPERATION);

	/* The error INVALID_ENUM is generated by .., VertexAttribIPointer,
	 * VertexAttribLPointer, ... if <type> is UNSIGNED_INT_10F_11F_11F_REV.
	 */

	if (piglit_get_gl_version() >= 30) {
		glVertexAttribIPointer(0, 3, GL_UNSIGNED_INT_10F_11F_11F_REV, 0, (GLvoid *)0);
		TEST("VertexAttribIPointer-not-allowed", GL_INVALID_ENUM);
	}

	if (piglit_is_extension_supported("GL_ARB_vertex_attrib_64bit")) {
		glVertexAttribLPointer(0, 3, GL_UNSIGNED_INT_10F_11F_11F_REV, 0, (GLvoid *)0);
		TEST("VertexAttribLPointer-not-allowed", GL_INVALID_ENUM);
	}

	/* The error INVALID_ENUM is generated by ... VertexAttribP4ui, VertexAttribP4uiv
	 * if <type> is UNSIGNED_INT_10F_11F_11F_REV
	 */

	glVertexAttribP4ui(0, GL_UNSIGNED_INT_10F_11F_11F_REV, GL_FALSE, 0);
	TEST("VertexAttribP4ui-not-allowed", GL_INVALID_ENUM);

	glVertexAttribP4uiv(0, GL_UNSIGNED_INT_10F_11F_11F_REV, GL_FALSE, &data);
	TEST("VertexAttribP4uiv-not-allowed", GL_INVALID_ENUM);

	glVertexAttribP1ui(0, GL_UNSIGNED_INT_10F_11F_11F_REV, GL_FALSE, 0);
	TEST("VertexAttribP1ui-ok", GL_NO_ERROR);

	glVertexAttribP1uiv(0, GL_UNSIGNED_INT_10F_11F_11F_REV, GL_FALSE, &data);
	TEST("VertexAttribP1uiv-ok", GL_NO_ERROR);

	glVertexAttribP2ui(0, GL_UNSIGNED_INT_10F_11F_11F_REV, GL_FALSE, 0);
	TEST("VertexAttribP2ui-ok", GL_NO_ERROR);

	glVertexAttribP2uiv(0, GL_UNSIGNED_INT_10F_11F_11F_REV, GL_FALSE, &data);
	TEST("VertexAttribP2uiv-ok", GL_NO_ERROR);

	glVertexAttribP3ui(0, GL_UNSIGNED_INT_10F_11F_11F_REV, GL_FALSE, 0);
	TEST("VertexAttribP3ui-ok", GL_NO_ERROR);

	glVertexAttribP3uiv(0, GL_UNSIGNED_INT_10F_11F_11F_REV, GL_FALSE, &data);
	TEST("VertexAttribP3uiv-ok", GL_NO_ERROR);

	/* Test that normalized = GL_TRUE doesn't spuriously produce an error */
	glVertexAttribP3uiv(0, GL_UNSIGNED_INT_10F_11F_11F_REV, GL_TRUE, &data);
	TEST("VertexAttribP3uiv-normalized-ok", GL_NO_ERROR);

	return pass;
}


void
piglit_init(int argc, char **argv)
{
	bool pass = true;
	GLuint bo;

	piglit_require_extension("GL_ARB_vertex_type_10f_11f_11f_rev");

	/* a small temporary bo for tests to work with. content doesn't matter,
	 * we won't be rendering from it. */
	glGenBuffers(1, &bo);
	glBindBuffer(GL_ARRAY_BUFFER, bo);
	glBufferData(GL_ARRAY_BUFFER, 64, (GLvoid const *)0, GL_STATIC_DRAW);

	if (piglit_is_extension_supported("GL_ARB_vertex_attrib_binding")) {
		pass = test_vertex_attrib_format() && pass;
	}

	pass = test_legacy_vertex_attribs() && pass;
	pass = test_vertex_attribs() && pass;

	piglit_report_result(pass ? PIGLIT_PASS : PIGLIT_FAIL);
}
