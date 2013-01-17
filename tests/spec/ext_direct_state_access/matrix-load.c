#include "piglit-util-gl-common.h"
#include "minmax-test.h"

PIGLIT_GL_TEST_CONFIG_BEGIN

    config.supports_gl_compat_version = 10;
    config.window_visual = PIGLIT_GL_VISUAL_RGB;

PIGLIT_GL_TEST_CONFIG_END

void
piglit_init(int argc, char **argv)
{
    float data[] = {
         1, 2, 3, 4,
         5, 6, 7, 8,
         9,10,11,12,
        13,14,15,16
    };

    float identity[] = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };

    float test[16];
    int mode, i;

    piglit_require_extension("GL_EXT_direct_state_access");

    /* test that glMatrixLoadfEXT() correctly loads a matrix
     * without disturbing the selector state */

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMatrixLoadfEXT(GL_PROJECTION, data);
    if (!piglit_check_gl_error(GL_NO_ERROR))
        piglit_report_result(PIGLIT_FAIL);

    glGetIntegerv(GL_MATRIX_MODE, &mode);
    if (mode != GL_MODELVIEW) {
        printf("glMatrixLoadfEXT disturbed the selector\n");
        piglit_report_result(PIGLIT_FAIL);
    }

    glGetFloatv(GL_MODELVIEW_MATRIX, test);
    for (i=0; i<16; i++)
        if (test[i] != identity[i]) {
            printf("glMatrixLoadfEXT disturbed the current matrix\n");
            piglit_report_result(PIGLIT_FAIL);
        }

    glGetFloatv(GL_PROJECTION_MATRIX, test);
    for (i=0; i<16; i++)
        if (test[i] != data[i]) {
            printf("glMatrixLoadfEXT failed to load the matrix correctly\n");
            piglit_report_result(PIGLIT_FAIL);
        }

    piglit_report_result(PIGLIT_PASS);
}

enum piglit_result
piglit_display(void)
{
    return PIGLIT_FAIL;
}
