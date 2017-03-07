#version 410 core

layout (location = 0) out vec4 fragColour;

in vec3 normalOut;
in vec2 UVout;

void main()
{
    fragColour = vec4(0.0f, 0.0f, 1.0f, 0.5f);
}
