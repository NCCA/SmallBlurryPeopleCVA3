#version 410 core

layout(location = 0) out int outID;

uniform int id;

void main()
{
    outID = id + 1;
}
