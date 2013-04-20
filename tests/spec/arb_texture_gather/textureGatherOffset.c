/*
 * Copyright © 2013 Chris Forbes
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
#include "piglit-util-gl-common.h"

PIGLIT_GL_TEST_CONFIG_BEGIN

    config.supports_gl_compat_version = 10;
    config.window_width = 64;
    config.window_height = 64;
    config.window_visual = PIGLIT_GL_VISUAL_RGBA | PIGLIT_GL_VISUAL_DOUBLE;

PIGLIT_GL_TEST_CONFIG_END

static void
fill_texture(int w, int h, int dx, int dy, int chan)
{
    float *data = malloc(sizeof(int) * w * h * 4);
    float *p = data;
    int i,j,k;

    for (j=0; j<h; j++)
        for (i=0; i<w; i++)
            for (k=0; k<4; k++)
                *p++ = (k == chan) ? (dy*j + dx*i)/64.0f : 0;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
            w, h, 0, GL_RGBA, GL_FLOAT, data);

    if (!piglit_check_gl_error(GL_NO_ERROR)) {
        printf("Failed to upload texture\n");
        piglit_report_result(PIGLIT_FAIL);
    }

    free(data);
}

static char const *swizzles[] = {"red","green","blue","alpha","zero","one"};
static GLenum swizzleEnums[] = {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA, GL_ZERO, GL_ONE};

static enum piglit_result
test_swizzle(int chan, GLenum stage)
{
    char const *swizzle = swizzles[chan];
    GLuint shader, prog;
    float half_gray[] = {0.5,0.5,0.5,0.5};
    GLuint tex[2];

    glGenTextures(2, tex);
    if (chan != 0 && !piglit_is_extension_supported("GL_ARB_texture_swizzle"))
        return PIGLIT_SKIP;

    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, tex[0]);
    fill_texture(64, 64, 1, 0, chan > 3 ? 0 : chan);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    if (chan != 0) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, swizzleEnums[chan]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_ZERO);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_ZERO);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ZERO);
    }

    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, tex[1]);
    fill_texture(64, 64, 0, 1, chan > 3 ? 0 : chan);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    if (chan != 0) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, swizzleEnums[chan]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_ZERO);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_ZERO);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ZERO);
    }

    if (stage == GL_FRAGMENT_SHADER) {
        shader = piglit_compile_shader_text(GL_FRAGMENT_SHADER,
            "#version 130\n"
            "#extension GL_ARB_texture_gather: require\n"
            "\n"
            "uniform sampler2D s0;\n"
            "uniform sampler2D s1;\n"
            "const ivec2 offset = ivec2(-8,7);\n"
            "\n"
            "vec4 check(sampler2D s, vec2 tc)\n"
            "{\n"
            "   vec4 g = textureGatherOffset(s, tc, offset);\n"
            "   float ofs = 1.0/128.0;\n"
            "   float v00 = textureOffset(s, tc, offset).r;\n"
            "   float v10 = textureOffset(s, tc + vec2(ofs,0), offset).r;\n"
            "   float v01 = textureOffset(s, tc + vec2(0,ofs), offset).r;\n"
            "   float v11 = textureOffset(s, tc + vec2(ofs,ofs), offset).r;\n"
            "   \n"
            "   return 0.5 + 60 * (g - vec4(v01,v11,v10,v00));\n"
            "}\n"
            "\n"
            "void main()\n"
            "{\n"
            "   vec2 tc = gl_FragCoord.xy / 64.0;\n"
            "   gl_FragColor = 0.5 * check(s0, tc) + 0.5 * check(s1, tc);\n"
            "}\n");
        prog = piglit_link_simple_program(0, shader);
    }
    else if (stage == GL_VERTEX_SHADER) {
        shader = piglit_compile_shader_text(GL_VERTEX_SHADER,
            "#version 130\n"
            "#extension GL_ARB_texture_gather: require\n"
            "#extension GL_ARB_explicit_attrib_location: require\n"
            "\n"
            "uniform sampler2D s0;\n"
            "uniform sampler2D s1;\n"
            "\n"
            "layout(location=0) in vec2 pos;\n"
            "const ivec2 offset = ivec2(-8,7);\n"
            "\n"
            "vec4 check(sampler2D s, vec2 tc)\n"
            "{\n"
            "   vec4 g = textureGatherOffset(s, tc, offset);\n"
            "   float ofs = 1.0/128.0;\n"
            "   float v00 = textureOffset(s, tc, offset).r;\n"
            "   float v10 = textureOffset(s, tc + vec2(ofs,0), offset).r;\n"
            "   float v01 = textureOffset(s, tc + vec2(0,ofs), offset).r;\n"
            "   float v11 = textureOffset(s, tc + vec2(ofs,ofs), offset).r;\n"
            "   \n"
            "   return 0.5 + 60 * (g - vec4(v01,v11,v10,v00));\n"
            "}\n"
            "\n"
            "void main()\n"
            "{\n"
            "   vec2 tc = pos / 64.0;\n"
            "   gl_Position = vec4(pos/32.0 - 1, 0, 1);\n"
            "   gl_FrontColor = 0.5 * check(s0, tc) + 0.5 * check(s1, tc);\n"
            "}\n");
        prog = piglit_link_simple_program(shader, 0);
    }

    if (!shader || !prog) {
        printf("Failed to compile/link shader\n");
        return PIGLIT_FAIL;
    }

    glUseProgram(prog);

    glUniform1i(glGetUniformLocation(prog, "s0"), 0);
    glUniform1i(glGetUniformLocation(prog, "s1"), 1);

    glClearColor(0.2,0.2,0.2,0.2);
    glClear(GL_COLOR_BUFFER_BIT);

    if (stage == GL_FRAGMENT_SHADER)
        piglit_draw_rect(-1,-1,2,2);
    else {
        int i,j;
        glBegin(GL_POINTS);
        for (i=0; i<64; i++)
            for (j=0; j<64; j++)
                glVertex2f(i,j);
        glEnd();

    }
    if (!piglit_probe_rect_rgba(1, 1, piglit_width-2, piglit_height-2, half_gray))
        return PIGLIT_FAIL;

    return PIGLIT_PASS;
}

void
piglit_init(int argc, char **argv)
{
    piglit_require_extension("GL_ARB_texture_gather");
}

enum piglit_result
piglit_display(void)
{
    int i;
    char name[32];
    for (i=0; i<6; i++) {
        sprintf(name, "fs-%s", swizzles[i]);
        piglit_report_subtest_result(test_swizzle(i, GL_FRAGMENT_SHADER), name);
    }
    for (i=0; i<6; i++) {
        sprintf(name, "vs-%s", swizzles[i]);
        piglit_report_subtest_result(test_swizzle(i, GL_VERTEX_SHADER), name);
    }
    piglit_present_results();

    piglit_report_result(PIGLIT_PASS);
}
