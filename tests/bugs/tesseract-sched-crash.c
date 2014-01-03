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
 *
 * Authors:
 *	Chris Forbes <chrisf@ijw.co.nz>
 */

#include "piglit-util-gl-common.h"

PIGLIT_GL_TEST_CONFIG_BEGIN

	config.supports_gl_core_version = 33;

	config.window_visual = PIGLIT_GL_VISUAL_DOUBLE | PIGLIT_GL_VISUAL_RGB | PIGLIT_GL_VISUAL_DEPTH;

PIGLIT_GL_TEST_CONFIG_END

enum piglit_result
piglit_display(void)
{
	glClearColor(1, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
       	return PIGLIT_PASS;
}

void piglit_init(int argc, char **argv)
{
	GLuint prog;
	GLuint vao;

	piglit_require_extension("GL_ARB_texture_gather");

	/* setup vao; we're in core profile so we must have one */
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	prog = piglit_build_simple_program(
	"#version 330\n"
	"#extension GL_ARB_texture_gather : enable\n"
	"layout(location=0) in vec4 vvertex;\n"
	"uniform mat4 lightmatrix;\n"
	"void main(void)\n"
	"{\n"
	"    gl_Position = lightmatrix * vvertex;\n"
	"}\n",

	"#version 330\n"
	"#extension GL_ARB_texture_gather : enable\n"
	"uniform sampler2DMS tex0, tex1, tex2, tex3;\n"
	"uniform sampler2D tex4;\n"
	"uniform vec4 lightpos[7];\n"
	"uniform vec3 lightcolor[7];\n"
	"uniform vec4 shadowparams[7];\n"
	"uniform vec2 shadowoffset[7];\n"
	"uniform vec3 camera;\n"
	"uniform mat4 worldmatrix;\n"
	"uniform vec2 fogdensity;\n"
	"uniform vec2 shadowatlasscale;\n"
	"uniform vec3 gdepthunpackparams;\n"
	"\n"
	"layout(location=0) out vec4 fragcolor;\n"
	"\n"
"                vec3 getshadowtc(vec3 dir, vec4 shadowparams, vec2 shadowoffset, float distbias)\n"
"                {\n"
"                    vec3 adir = abs(dir);\n"
"                    float m = max(adir.x, adir.y), mz = max(adir.z, m);\n"
"                    vec2 mparams = shadowparams.xy / max(mz + distbias, 1e-5);\n"
"                    vec4 proj;\n"
"                    if(adir.x > adir.y) proj = vec4(dir.zyx, 0.0); else proj = vec4(dir.xzy, 1.0);\n"
"                    if(adir.z > m) proj = vec4(dir, 2.0);\n"
"                    return vec3(proj.xy * mparams.x + vec2(proj.w, step(proj.z, 0.0)) * shadowparams.z + shadowoffset, mparams.y + shadowparams.w);\n"
"                }\n"
"            \n"
"\n"
"        \n"
"                \n"
"                #define shadowgather(center, xoff, yoff) step(shadowtc.z, textureGatherOffset(tex4, center, ivec2(xoff, yoff)))\n"
"                \n"
"                float filtershadow(vec3 shadowtc)\n"
"                {\n"
"                    vec2 offset = fract(shadowtc.xy - 0.5), center = (shadowtc.xy - offset)*shadowatlasscale;\n"
"                    vec4 group1 = shadowgather(center, -1, -1);\n"
"                    vec4 group2 = shadowgather(center,  1, -1);\n"
"                    vec4 group3 = shadowgather(center, -1,  1);\n"
"                    vec4 group4 = shadowgather(center,  1,  1);\n"
"                    vec4 cols = vec4(group1.rg, group2.rg) + vec4(group3.ab, group4.ab) + mix(vec4(group1.ab, group2.ab), vec4(group3.rg, group4.rg), offset.y);\n"
"                    return dot(mix(cols.xyz, cols.yzw, offset.x), vec3(1.0/9.0));\n"
"                }\n"
"            \n"
"\n"
"        \n"
"            \n"
"        void main(void)\n"
"        {\n"
"            \n"
"                    \n"
"\n"
"                    #define gfetch(sampler, coords) texelFetch(sampler, ivec2(coords), 0)\n"
"\n"
"                    #define accumlight(light) fragcolor.rgb = light\n"
"                    #define accumalpha(alpha) fragcolor.a = alpha\n"
"                \n"
"\n"
"            \n"
"                \n"
"                    vec4 depthvals = gfetch(tex3, gl_FragCoord.xy);\n"
"                \n"
"                \n"
"            float depth = dot(depthvals.rgb, gdepthunpackparams); \n"
"            \n"
"                    \n"
"                        vec3 pos = (worldmatrix * vec4(depth*gl_FragCoord.xy, depth, 1.0)).xyz;\n"
"                    \n"
"                \n"
"        \n"
"                \n"
"                    vec4 normal = gfetch(tex1, gl_FragCoord.xy);\n"
"                    normal.xyz = normal.xyz*2.0 - 1.0;\n"
"                \n"
"                \n"
"                    float fogcoord = length(camera - pos.xyz);\n"
"                \n"
"                \n"
"                    \n"
"                        float distbias = -8.0*step(fogcoord, 8.0);\n"
"                        distbias *= depthvals.a;\n"
"                    \n"
"                    vec3 camdir = normalize(camera - pos.xyz);\n"
"                    float facing = 2.0*dot(normal.xyz, camdir);\n"
"                \n"
"                \n"
"            \n"
"\n"
"            \n"
"                vec4 diffuse = gfetch(tex0, gl_FragCoord.xy);\n"
"            \n"
"            \n"
"                vec3 light = vec3(0.0);\n"
"            \n"
"\n"
"            \n"
"\n"
"            \n"
"                vec3 light0dir = pos.xyz * lightpos[0].w - lightpos[0].xyz;\n"
"                float light0dist2 = dot(light0dir, light0dir);\n"
"                if(light0dist2 < 1.0)\n"
"                {\n"
"                    \n"
"                    float light0facing = dot(light0dir, normal.xyz);\n"
"                    if(light0facing < 0.0) \n"
"                    {\n"
"                        float light0invdist = inversesqrt(light0dist2); \n"
"                        \n"
"                        float light0atten = light0facing * (1.0 - light0invdist);\n"
"                        \n"
"                        \n"
"                        \n"
"                                vec3 shadow0tc = getshadowtc(light0dir, shadowparams[0], shadowoffset[0], distbias * lightpos[0].w);\n"
"                                light0atten *= filtershadow(shadow0tc);\n"
"                            \n"
"                        \n"
"                        \n"
"                            \n"
"                            float light0spec = pow(clamp(light0invdist*(dot(camdir, light0dir) - light0facing*facing), 0.0, 1.0), 16.0) * diffuse.a;\n"
"                            light += (diffuse.rgb + light0spec) * lightcolor[0] * light0atten;\n"
"                            \n"
"                        \n"
"                         \n"
"                    }\n"
"                }\n"
"             \n"
"                vec3 light1dir = pos.xyz * lightpos[1].w - lightpos[1].xyz;\n"
"                float light1dist2 = dot(light1dir, light1dir);\n"
"                if(light1dist2 < 1.0)\n"
"                {\n"
"                    \n"
"                    float light1facing = dot(light1dir, normal.xyz);\n"
"                    if(light1facing < 0.0) \n"
"                    {\n"
"                        float light1invdist = inversesqrt(light1dist2); \n"
"                        \n"
"                        float light1atten = light1facing * (1.0 - light1invdist);\n"
"                        \n"
"                        \n"
"                        \n"
"                                vec3 shadow1tc = getshadowtc(light1dir, shadowparams[1], shadowoffset[1], distbias * lightpos[1].w);\n"
"                                light1atten *= filtershadow(shadow1tc);\n"
"                            \n"
"                        \n"
"                        \n"
"                            \n"
"                            float light1spec = pow(clamp(light1invdist*(dot(camdir, light1dir) - light1facing*facing), 0.0, 1.0), 16.0) * diffuse.a;\n"
"                            light += (diffuse.rgb + light1spec) * lightcolor[1] * light1atten;\n"
"                            \n"
"                        \n"
"                         \n"
"                    }\n"
"                }\n"
"             \n"
"                vec3 light2dir = pos.xyz * lightpos[2].w - lightpos[2].xyz;\n"
"                float light2dist2 = dot(light2dir, light2dir);\n"
"                if(light2dist2 < 1.0)\n"
"                {\n"
"                    \n"
"                    float light2facing = dot(light2dir, normal.xyz);\n"
"                    if(light2facing < 0.0) \n"
"                    {\n"
"                        float light2invdist = inversesqrt(light2dist2); \n"
"                        \n"
"                        float light2atten = light2facing * (1.0 - light2invdist);\n"
"                        \n"
"                        \n"
"                        \n"
"                                vec3 shadow2tc = getshadowtc(light2dir, shadowparams[2], shadowoffset[2], distbias * lightpos[2].w);\n"
"                                light2atten *= filtershadow(shadow2tc);\n"
"                            \n"
"                        \n"
"                        \n"
"                            \n"
"                            float light2spec = pow(clamp(light2invdist*(dot(camdir, light2dir) - light2facing*facing), 0.0, 1.0), 16.0) * diffuse.a;\n"
"                            light += (diffuse.rgb + light2spec) * lightcolor[2] * light2atten;\n"
"                            \n"
"                        \n"
"                         \n"
"                    }\n"
"                }\n"
"             \n"
"                vec3 light3dir = pos.xyz * lightpos[3].w - lightpos[3].xyz;\n"
"                float light3dist2 = dot(light3dir, light3dir);\n"
"                if(light3dist2 < 1.0)\n"
"                {\n"
"                    \n"
"                    float light3facing = dot(light3dir, normal.xyz);\n"
"                    if(light3facing < 0.0) \n"
"                    {\n"
"                        float light3invdist = inversesqrt(light3dist2); \n"
"                        \n"
"                        float light3atten = light3facing * (1.0 - light3invdist);\n"
"                        \n"
"                        \n"
"                        \n"
"                                vec3 shadow3tc = getshadowtc(light3dir, shadowparams[3], shadowoffset[3], distbias * lightpos[3].w);\n"
"                                light3atten *= filtershadow(shadow3tc);\n"
"                            \n"
"                        \n"
"                        \n"
"                            \n"
"                            float light3spec = pow(clamp(light3invdist*(dot(camdir, light3dir) - light3facing*facing), 0.0, 1.0), 16.0) * diffuse.a;\n"
"                            light += (diffuse.rgb + light3spec) * lightcolor[3] * light3atten;\n"
"                            \n"
"                        \n"
"                         \n"
"                    }\n"
"                }\n"
"             \n"
"                vec3 light4dir = pos.xyz * lightpos[4].w - lightpos[4].xyz;\n"
"                float light4dist2 = dot(light4dir, light4dir);\n"
"                if(light4dist2 < 1.0)\n"
"                {\n"
"                    \n"
"                    float light4facing = dot(light4dir, normal.xyz);\n"
"                    if(light4facing < 0.0) \n"
"                    {\n"
"                        float light4invdist = inversesqrt(light4dist2); \n"
"                        \n"
"                        float light4atten = light4facing * (1.0 - light4invdist);\n"
"                        \n"
"                        \n"
"                        \n"
"                                vec3 shadow4tc = getshadowtc(light4dir, shadowparams[4], shadowoffset[4], distbias * lightpos[4].w);\n"
"                                light4atten *= filtershadow(shadow4tc);\n"
"                            \n"
"                        \n"
"                        \n"
"                            \n"
"                            float light4spec = pow(clamp(light4invdist*(dot(camdir, light4dir) - light4facing*facing), 0.0, 1.0), 16.0) * diffuse.a;\n"
"                            light += (diffuse.rgb + light4spec) * lightcolor[4] * light4atten;\n"
"                            \n"
"                        \n"
"                         \n"
"                    }\n"
"                }\n"
"             \n"
"                vec3 light5dir = pos.xyz * lightpos[5].w - lightpos[5].xyz;\n"
"                float light5dist2 = dot(light5dir, light5dir);\n"
"                if(light5dist2 < 1.0)\n"
"                {\n"
"                    \n"
"                    float light5facing = dot(light5dir, normal.xyz);\n"
"                    if(light5facing < 0.0) \n"
"                    {\n"
"                        float light5invdist = inversesqrt(light5dist2); \n"
"                        \n"
"                        float light5atten = light5facing * (1.0 - light5invdist);\n"
"                        \n"
"                        \n"
"                        \n"
"                                vec3 shadow5tc = getshadowtc(light5dir, shadowparams[5], shadowoffset[5], distbias * lightpos[5].w);\n"
"                                light5atten *= filtershadow(shadow5tc);\n"
"                            \n"
"                        \n"
"                        \n"
"                            \n"
"                            float light5spec = pow(clamp(light5invdist*(dot(camdir, light5dir) - light5facing*facing), 0.0, 1.0), 16.0) * diffuse.a;\n"
"                            light += (diffuse.rgb + light5spec) * lightcolor[5] * light5atten;\n"
"                            \n"
"                        \n"
"                         \n"
"                    }\n"
"                }\n"
"             \n"
"                vec3 light6dir = pos.xyz * lightpos[6].w - lightpos[6].xyz;\n"
"                float light6dist2 = dot(light6dir, light6dir);\n"
"                if(light6dist2 < 1.0)\n"
"                {\n"
"                    \n"
"                    float light6facing = dot(light6dir, normal.xyz);\n"
"                    if(light6facing < 0.0) \n"
"                    {\n"
"                        float light6invdist = inversesqrt(light6dist2); \n"
"                        \n"
"                        float light6atten = light6facing * (1.0 - light6invdist);\n"
"                        \n"
"                        \n"
"                        \n"
"                                vec3 shadow6tc = getshadowtc(light6dir, shadowparams[6], shadowoffset[6], distbias * lightpos[6].w);\n"
"                                light6atten *= filtershadow(shadow6tc);\n"
"                            \n"
"                        \n"
"                        \n"
"                            \n"
"                            float light6spec = pow(clamp(light6invdist*(dot(camdir, light6dir) - light6facing*facing), 0.0, 1.0), 16.0) * diffuse.a;\n"
"                            light += (diffuse.rgb + light6spec) * lightcolor[6] * light6atten;\n"
"                            \n"
"                        \n"
"                         \n"
"                    }\n"
"                }\n"
"            \n"
"            \n"
"                float foglerp = clamp(exp2(fogcoord*fogdensity.x)*fogdensity.y, 0.0, 1.0);\n"
"                \n"
"                    accumlight(light*foglerp);\n"
"                    accumalpha(0.0);\n"
"                \n"
"            \n"
"\n"
"            \n"
"        }");

	if (!prog) {
		piglit_report_result(PIGLIT_FAIL);
	}

	glUseProgram(prog);

	/* setup required surfaces: G-buffer is 4x RGBA8 MSAA surfaces. */
	{
		GLuint mstex[4];
		char const *names[] = { "tex0", "tex1", "tex2", "tex3" };
		int i;

		for (i=0; i<4; i++) {
			glGenTextures(1, &mstex[i]);
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mstex[i]);
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4,
					GL_RGBA8, 256, 256, GL_TRUE);

			glUniform1i(glGetUniformLocation(prog, names[i]), i);
		}
	}

	/* misc surface -- shadow buffer */
	{
		GLuint shadow_tex;
		glGenTextures(1, &shadow_tex);
		glActiveTexture(GL_TEXTURE0 + 4);
		glBindTexture(GL_TEXTURE_2D, shadow_tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0,
				GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		glUniform1i(glGetUniformLocation(prog, "tex4"), 4);
	}
	
	/* setup geometry */
	{
		GLuint bo;
		glGenBuffers(1, &bo);
		glBindBuffer(GL_ARRAY_BUFFER, bo);
		glBufferData(GL_ARRAY_BUFFER, 48, NULL, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}
}
