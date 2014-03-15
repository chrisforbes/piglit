#ifndef VIEW_CLASSES_H
#define VIEW_CLASSES_H

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
 * View classes for ARB_texture_view format consistency tests.
 */

struct format_info {
	GLenum internalformat;
	GLenum pixelformat;
	GLenum pixeltype;
	char *sampler_prefix;
};

struct format_info view_class_128bits[] = {
	{ GL_RGBA32F,	GL_RGBA,		GL_FLOAT,		"" },
	{ GL_RGBA32UI,	GL_RGBA_INTEGER,	GL_UNSIGNED_INT,	"u" },
	{ GL_RGBA32I,	GL_RGBA_INTEGER,	GL_INT,			"i" },
	{ 0 },
};

struct format_info view_class_96bits[] = {
	{ GL_RGB32F,	GL_RGB,			GL_FLOAT,		"" },
	{ GL_RGB32UI,	GL_RGB_INTEGER,		GL_UNSIGNED_INT,	"u" },
	{ GL_RGB32I,	GL_RGB_INTEGER,		GL_INT,			"i" },
	{ 0 },
};

struct format_info view_class_64bits[] = {
	{ GL_RGBA16F,	GL_RGBA,		GL_HALF_FLOAT,		"" },
	{ GL_RGBA16UI,	GL_RGBA_INTEGER,	GL_UNSIGNED_SHORT,	"u" },
	{ GL_RGBA16I,	GL_RGBA_INTEGER,	GL_SHORT,		"i" },
	{ GL_RGBA16,	GL_RGBA,		GL_UNSIGNED_SHORT,	"" },
	{ GL_RG32F,	GL_RG,			GL_FLOAT,		"" },
	{ GL_RG32UI,	GL_RG_INTEGER,		GL_UNSIGNED_INT,	"u" },
	{ GL_RG32I,	GL_RG_INTEGER,		GL_INT,			"i" },
	{ GL_RGBA16_SNORM, GL_RGBA,		GL_SHORT,		"" },
	{ 0 },
};

struct format_info view_class_48bits[] = {
	{ GL_RGB16F,	GL_RGB,			GL_HALF_FLOAT,		"" },
	{ GL_RGB16,	GL_RGB,			GL_UNSIGNED_SHORT,	"" },
	{ GL_RGB16UI,	GL_RGB_INTEGER,		GL_UNSIGNED_SHORT,	"u" },
	{ GL_RGB16I,	GL_RGB_INTEGER,		GL_SHORT,		"i" },
	{ GL_RGB16_SNORM, GL_RGB,		GL_SHORT,		"" },
	{ 0 },
};

struct format_info view_class_32bits[] = {
	{ GL_RG16F,	GL_RG,			GL_HALF_FLOAT,		"" },
	{ GL_R11F_G11F_B10F, GL_RGB,		GL_UNSIGNED_INT_10F_11F_11F_REV, "" },
	{ GL_R32F,	GL_RED,			GL_FLOAT,		"" },
	{ GL_RGB10_A2UI, GL_RGBA_INTEGER,	GL_UNSIGNED_INT_2_10_10_10_REV, "u" },
	{ GL_RGBA8UI,	GL_RGBA_INTEGER,	GL_UNSIGNED_BYTE,	"u" },
	{ GL_RG16UI,	GL_RG_INTEGER,		GL_UNSIGNED_SHORT,	"u" },
	{ GL_R32UI,	GL_RED_INTEGER,		GL_UNSIGNED_INT,	"u" },
	{ GL_RGBA8I,	GL_RGBA_INTEGER,	GL_BYTE,		"i" },
	{ GL_RG16I,	GL_RG_INTEGER,		GL_SHORT,		"i" },
	{ GL_R32I,	GL_RED_INTEGER,		GL_INT,			"i" },
	{ GL_RGB10_A2,	GL_RGBA,		GL_UNSIGNED_INT_2_10_10_10_REV, "" },
	{ GL_RGBA8,	GL_RGBA,		GL_UNSIGNED_BYTE,	"" },
	{ GL_RG16,	GL_RG,			GL_UNSIGNED_SHORT,	"" },
	{ GL_RGBA8_SNORM, GL_RGBA,		GL_BYTE,		"" },
	{ GL_RG16_SNORM, GL_RG,			GL_SHORT,		"" },
	{ GL_SRGB8_ALPHA8, GL_RGBA,		GL_UNSIGNED_BYTE,	"" },
	{ GL_RGB9_E5,	GL_RGB,			GL_UNSIGNED_INT_5_9_9_9_REV, "" },
	{ 0 },
};

struct format_info view_class_24bits[] = {
	{ GL_RGB8,	GL_RGB,			GL_UNSIGNED_BYTE,	"" },
	{ GL_RGB8_SNORM, GL_RGB,		GL_BYTE,		"" },
	{ GL_SRGB8,	GL_RGB,			GL_UNSIGNED_BYTE,	"" },
	{ GL_RGB8UI,	GL_RGB_INTEGER,		GL_UNSIGNED_BYTE,	"u" },
	{ GL_RGB8I,	GL_RGB_INTEGER,		GL_BYTE,		"i" },
	{ 0 },
};

struct format_info view_class_16bits[] = {
	{ GL_R16F,	GL_RED,			GL_HALF_FLOAT,		"" },
	{ GL_RG8UI,	GL_RG_INTEGER,		GL_UNSIGNED_BYTE,	"u" },
	{ GL_R16UI,	GL_RED_INTEGER,		GL_UNSIGNED_SHORT,	"u" },
	{ GL_RG8I,	GL_RG_INTEGER,		GL_BYTE,		"i" },
	{ GL_R16I,	GL_RED_INTEGER,		GL_SHORT,		"i" },
	{ GL_RG8,	GL_RG,			GL_UNSIGNED_BYTE,	"" },
	{ GL_R16,	GL_RED,			GL_UNSIGNED_SHORT,	"" },
	{ GL_RG8_SNORM,	GL_RG,			GL_BYTE,		"" },
	{ GL_R16_SNORM,	GL_RED,			GL_SHORT,		"" },
	{ 0 },
};

struct format_info view_class_8bits[] = {
	{ GL_R8UI,	GL_RED_INTEGER,		GL_UNSIGNED_BYTE,	"u" },
	{ GL_R8I,	GL_RED_INTEGER,		GL_BYTE,		"i" },
	{ GL_R8,	GL_RED,			GL_UNSIGNED_BYTE,	"" },
	{ GL_R8_SNORM,	GL_RED,			GL_BYTE,		"" },
	{ 0 },
};

#endif
