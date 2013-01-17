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
int sample_count;

struct sample_info
{
    float color[4];
    float draw[4];
    int probe[2];
};

struct sample_info samples[] = {
    { {1,0,0,1}, {-1,-1,1,1}, {16,16} },
    { {0,1,0,1}, { 0,-1,1,1}, {48,16} },
    { {0,0,1,1}, { 0, 0,1,1}, {48,48} },
    { {1,1,1,1}, {-1, 0,1,1}, {16,48} },
};

enum piglit_result
piglit_display(void)
{
    bool pass = true;

    int i;

    glClearColor(0.2,0.2,0.2,1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(prog);

    for (i=0; i < sample_count; i++) {
        struct sample_info *s = &samples[i];
        glUniform1i(u_sample, i);
        piglit_draw_rect(s->draw[0], s->draw[1], s->draw[2], s->draw[3]);
    }

    for (i=0; i < sample_count; i++) {
        struct sample_info *s = &samples[i];
        pass = piglit_probe_pixel_rgba(s->probe[0], s->probe[1], s->color);
    }

    piglit_present_results();

    return pass ? PIGLIT_PASS : PIGLIT_FAIL;
}

void
usage(int argc, char **argv)
{
    printf("usage: %s vs|fs samplecount\n", argv[0]);
    piglit_report_result(PIGLIT_SKIP);
}

void
piglit_init(int argc, char **argv)
{
    int i;
    int max_samples;
    sample_count = 0;

    piglit_require_extension("GL_ARB_texture_multisample");

    if (argc != 3)
        usage(argc, argv);

    if (!strcmp(argv[1], "vs"))
        shader_stage = GL_VERTEX_SHADER;
    else if (!strcmp(argv[1], "fs"))
        shader_stage = GL_FRAGMENT_SHADER;
    else
        usage(argc, argv);

    sample_count = atoi(argv[2]);
    glGetIntegerv(GL_MAX_SAMPLES, &max_samples);

    if (sample_count > max_samples) {
        printf("sample count of %d not supported.\n",
                sample_count);
        piglit_report_result(PIGLIT_SKIP);
    }

    if (sample_count > sizeof(samples) / sizeof(*samples)) {
        printf("sample count of %d not supported by this test, extend it\n",
                sample_count);
        piglit_report_result(PIGLIT_SKIP);
    }

    glGenFramebuffers(1, &fbo);
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tex);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE,
                            sample_count, GL_RGBA, 64, 64, GL_TRUE);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D_MULTISAMPLE, tex, 0);

    glClearColor(0.2,0.2,0.2,1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    /* write test pattern: red,green,blue,white */
    glEnable(GL_SAMPLE_MASK);

    for (i=0; i<sample_count; i++) {
        struct sample_info *s = &samples[i];
        glSampleMaski(0, (1<<i));
        glColor4fv(s->color);
        piglit_draw_rect(-1,-1,2,2);
    }

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
        assert(0);
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
