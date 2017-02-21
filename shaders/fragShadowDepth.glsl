#version 410 core

layout (location = 0) out float fdepth;

void main()
{
    fdepth = gl_FragCoord.z;
    //fdepth = 0.0f;
}
