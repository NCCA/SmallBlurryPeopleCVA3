#version 410 core

layout(location = 0) out vec4 outPosition;

in vec4 position;

void main()
{
    outPosition = vec4(position.xyz, 1.0);
    //outID = inID;
}
