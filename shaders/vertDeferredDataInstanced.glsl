#version 410 core

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;

struct transformation
{
    mat4 MVP;
    mat4 M;
};

layout( std140 ) uniform transformationBuffer
{
    transformation t [512];
} tbuf;

out vec4 normal;
out vec4 position;
flat out int id;
out vec2 UV;

uniform int ID;

void main()
{
    gl_Position = tbuf.t[gl_InstanceID].MVP * inPosition;

    normal = tbuf.t[gl_InstanceID].M * vec4(inNormal.xyz, 0.0);
    normal.w = 1.0;
    position = tbuf.t[gl_InstanceID].M * inPosition;
    id = ID;
    UV = inUV;
}
