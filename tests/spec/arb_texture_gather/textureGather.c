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
    config.window_visual = PIGLIT_GL_VISUAL_RGBA | PIGLIT_GL_VISUAL_ALPHA | PIGLIT_GL_VISUAL_DOUBLE;

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

void
piglit_init(int argc, char **argv)
{
    GLuint tex[2], fs, prog;
    int i,j;
    char const *shader;
    char const *swizzle = "r";
    piglit_require_extension("GL_ARB_texture_gather");

    glGenTextures(2, tex);

    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, tex[0]);
    fill_texture(64, 64, 1, 0, 0 /* red channel */);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, tex[1]);
    fill_texture(64, 64, 0, 1, 0 /* red channel */);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    asprintf(&shader,
        "#version 130\n"
        "#extension GL_ARB_texture_gather: require\n"
        "\n"
        "uniform sampler2D s0;\n"
        "uniform sampler2D s1;\n"
        "\n"
        "vec4 check(sampler2D s, vec2 tc)\n"
        "{\n"
        "   vec4 g = textureGather(s, tc);\n"
        "   float ofs = 1.0/128.0;\n"
        "   float v00 = texture(s, tc).%s;\n"
        "   float v10 = texture(s, tc + vec2(ofs,0)).%s;\n"
        "   float v01 = texture(s, tc + vec2(0,ofs)).%s;\n"
        "   float v11 = texture(s, tc + vec2(ofs,ofs)).%s;\n"
        "   \n"
        "   return 0.5 + 60 * (g - vec4(v01,v11,v10,v00));\n"
        "}\n"
        "\n"
        "void main()\n"
        "{\n"
        "   vec2 tc = gl_FragCoord.xy / 64.0;\n"
        "   gl_FragColor = 0.5 * check(s0, tc) + 0.5 * check(s1, tc);\n"
        "}\n",
        swizzle, swizzle, swizzle, swizzle);

    fs = piglit_compile_shader_text(GL_FRAGMENT_SHADER, shader);
    prog = piglit_link_simple_program(0, fs);
    if (!fs || !prog) {
        printf("Failed to compile/link shader\n");
        piglit_report_result(PIGLIT_FAIL);
    }

    glUseProgram(prog);

    glUniform1i(glGetUniformLocation(prog, "s0"), 0);
    glUniform1i(glGetUniformLocation(prog, "s1"), 1);
}

enum piglit_result
piglit_display(void)
{
    float half_gray[] = {0.5,0.5,0.5,0.5};

    glClearColor(0.2,0.2,0.2,0.2);
    glClear(GL_COLOR_BUFFER_BIT);
    piglit_draw_rect(-1,-1,2,2);
    if (!piglit_probe_rect_rgba(1, 1, piglit_width-2, piglit_height-2, half_gray))
        piglit_report_result(PIGLIT_FAIL);
    piglit_present_results();

    piglit_report_result(PIGLIT_PASS);
}
