#version 410 core

layout (location = 0) in vec4 inPosition;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inNormal;

//uniform mat4 gWorld;

out vec4 position_tc;
out vec2 uv_tc;
out vec3 normal_tc;

void main()
{
    position_tc = inPosition;
    uv_tc = inUV;
    normal_tc = inNormal;
}
