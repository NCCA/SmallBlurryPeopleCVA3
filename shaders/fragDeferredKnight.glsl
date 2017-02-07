#version 410 core

layout(location = 0) out vec4 outColour;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outPosition;
//layout(location = 3) out int outID;

uniform sampler2D diffuse;

in vec4 normal;
in vec4 position;
flat in int id;
in vec2 UV;

void main()
{
    outColour = texture(diffuse, vec2(UV.x, -UV.y));
    outNormal = normal;
    outPosition = position;
    //outID = inID;
}
