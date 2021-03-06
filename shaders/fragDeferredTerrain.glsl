#version 410 core

layout(location = 0) out vec4 outColour;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outPosition;
//layout(location = 3) out int outID;

flat in vec4 normal;
in vec4 position;
flat in int inID;
in vec2 UV;

void main()
{
    outColour = vec4(1.0);
    outNormal = normal;
    outPosition = position;
    //outID = inID;
}
