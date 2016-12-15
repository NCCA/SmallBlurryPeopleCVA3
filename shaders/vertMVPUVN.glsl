#version 430 core

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;

out vec2 UV;
out vec3 normal;

uniform mat4 M;
uniform mat4 MVP;
uniform mat3 normalMat;

void main()
{
    gl_Position = MVP * inPosition;
    //Weird, I seem to have to flip the y uv.
    UV = vec2(inUV.x, -inUV.y);
    normal = /*normalMat */ inNormal;
}
