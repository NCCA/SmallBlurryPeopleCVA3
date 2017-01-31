#version 430 core

layout(location = 0) out vec4 colour;
layout(location = 1) out vec4 normal;
layout(location = 2) out vec4 position;
layout(location = 3) out int id;

in vec4 outColour;
in vec4 outNormal;
in vec4 outPosition;
in int outID;

void main()
{
    colour = inColour;
    normal = inNormal;
    position = inPosition;
    id = inID;
}
