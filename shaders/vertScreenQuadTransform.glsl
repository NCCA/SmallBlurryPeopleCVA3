#version 410 core

layout( location = 0 ) in vec3 inPosition;
layout( location = 1 ) in vec2 inUV;

out vec2 UV;

uniform mat4 M;

void main()
{
    gl_Position = M * vec4(inPosition.xyz, 1.0);
    UV = inUV;
}
