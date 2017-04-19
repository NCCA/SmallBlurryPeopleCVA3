#version 410 core

layout(location = 0) out vec4 outColour;

in vec4 normal;
in vec4 position;
flat in int inID;
in vec2 UV;

void main()
{
    outColour = vec4(normal.xyz, 1.0);
}
