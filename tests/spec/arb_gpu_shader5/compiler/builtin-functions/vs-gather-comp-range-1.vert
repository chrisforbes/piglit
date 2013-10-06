// [config]
// expect_result: fail
// glsl_version: 1.50
// require_extensions: GL_ARB_gpu_shader5 GL_ARB_texture_cube_map_array
// [end config]

#version 150
#extension GL_ARB_gpu_shader5: require
#extension GL_ARB_texture_cube_map_array: require

uniform sampler2D s2D;

void main()
{
	gl_Position = textureGather(s2D, vec2(0), -1 /* out of range */);
}
