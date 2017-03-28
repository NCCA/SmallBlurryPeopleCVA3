#version 410 core

layout(location = 0) out vec4 outColour;

uniform vec4 colour;
uniform vec4 base;

in vec4 normal;
in vec4 position;
flat in int inID;
in vec2 UV;

void main()
{
    outColour = colour;
    //outColour.a = mix(1.0, 0.0, position.y - base.y);
}
