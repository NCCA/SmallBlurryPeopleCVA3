#version 410 core

layout( location = 0 ) in vec3 inPosition;
layout( location = 1 ) in vec2 inUV;

out vec2 UV;

void main()
{
    gl_Position = vec4(inPosition.xyz, 1.0);
    UV = inUV;
}
