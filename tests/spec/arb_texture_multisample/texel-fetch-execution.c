#include "piglit-util-gl-common.h"

PIGLIT_GL_TEST_CONFIG_BEGIN

    config.supports_gl_compat_version = 30;

    config.window_width = 64;
    config.window_height = 64;
    config.window_visual = PIGLIT_GL_VISUAL_RGB | PIGLIT_GL_VISUAL_ALPHA;

PIGLIT_GL_TEST_CONFIG_END

/* test texelFetch() from multisample textures.
 * instead of using actual multisample rasterization,
 * we'll write the test pattern to a MSAA texture via
 * sample masking.
 */

GLuint fbo, tex, shader, prog;
GLint u_sample;
GLuint shader_stage = 0;

enum piglit_result
piglit_display(void)
{
    float red[] = {1,0,0,1};
    float green[] = {0,1,0,1};
    float blue[] = {0,0,1,1};
    float white[] = {1,1,1,1};

    bool pass = true;

    glClearColor(0.2,0.2,0.2,1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(prog);

    glUniform1i(u_sample, 0);
    piglit_draw_rect(-1,-1,1,1);

    glUniform1i(u_sample, 1);
    piglit_draw_rect(0,-1,1,1);

    glUniform1i(u_sample, 2);
    piglit_draw_rect(0,0,1,1);

    glUniform1i(u_sample, 3);
    piglit_draw_rect(-1,0,1,1);

    pass = piglit_probe_pixel_rgba(16, 16, red) && pass;
    pass = piglit_probe_pixel_rgba(48, 16, green) && pass;
    pass = piglit_probe_pixel_rgba(48, 48, blue) && pass;
    pass = piglit_probe_pixel_rgba(16, 48, white) && pass;

    piglit_present_results();

    return pass ? PIGLIT_PASS : PIGLIT_FAIL;
}

void
piglit_init(int argc, char **argv)
{
    piglit_require_extension("GL_ARB_texture_multisample");

    while (++argv,--argc) {
        if (!strcmp(*argv, "vs"))
            shader_stage = GL_VERTEX_SHADER;
        else if (!strcmp(*argv, "fs"))
            shader_stage = GL_FRAGMENT_SHADER;
    }


    glGenFramebuffers(1, &fbo);
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tex);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE,
                            4, GL_RGBA, 64, 64, GL_TRUE);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D_MULTISAMPLE, tex, 0);

    glClearColor(0.2,0.2,0.2,1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    /* write test pattern: red,green,blue,white */
    glEnable(GL_SAMPLE_MASK);

    glSampleMaski(0, (1<<0));
    glColor4f(1.0,0.0,0.0,1.0);
    piglit_draw_rect(-1,-1,2,2);

    glSampleMaski(0, (1<<1));
    glColor4f(0.0,1.0,0.0,1.0);
    piglit_draw_rect(-1,-1,2,2);

    glSampleMaski(0, (1<<2));
    glColor4f(0.0,0.0,1.0,1.0);
    piglit_draw_rect(-1,-1,2,2);

    glSampleMaski(0, (1<<3));
    glColor4f(1.0,1.0,1.0,1.0);
    piglit_draw_rect(-1,-1,2,2);

    glDisable(GL_SAMPLE_MASK);

    if (!piglit_check_gl_error(GL_NO_ERROR))
        piglit_report_result(PIGLIT_FAIL);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    switch (shader_stage) {
    case GL_VERTEX_SHADER:
        shader = piglit_compile_shader_text(GL_VERTEX_SHADER,
            "#version 130\n"
            "#extension GL_ARB_texture_multisample : require\n"
            "\n"
            "uniform sampler2DMS s;\n"
            "uniform int sample;\n"
            "\n"
            "void main() {\n"
            "   gl_Position = ftransform();\n"
            "   gl_FrontColor = texelFetch(s, ivec2(32, 32), sample); \n"
            "}\n");

        prog = piglit_link_simple_program(shader, 0);
        break;

    case GL_FRAGMENT_SHADER:
        shader = piglit_compile_shader_text(GL_FRAGMENT_SHADER,
            "#version 130\n"
            "#extension GL_ARB_texture_multisample : require\n"
            "\n"
            "uniform sampler2DMS s;\n"
            "uniform int sample;\n"
            "\n"
            "void main() {\n"
            "   gl_FragColor = texelFetch(s, ivec2(32,32), sample); \n"
            "}\n");

        prog = piglit_link_simple_program(0, shader);
        break;

    default:
        printf("Please provide shader type: `vs` or `fs`\n");
        piglit_report_result(PIGLIT_FAIL);
    }

    if (!prog || !shader)
        piglit_report_result(PIGLIT_FAIL);

    u_sample = glGetUniformLocation(prog, "sample");
    printf("sample uniform: %d\n", u_sample);
    if (u_sample == -1)
        piglit_report_result(PIGLIT_FAIL);

    if (!piglit_check_gl_error(GL_NO_ERROR))
        piglit_report_result(PIGLIT_FAIL);
}
