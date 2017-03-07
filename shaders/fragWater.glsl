#version 410 core

layout (location = 0) out vec4 fragColour;

in vec4 position_fs;
in vec3 normal_fs;
in vec2 uv_fs;

void main()
{
    fragColour = vec4(0.1f, 0.2f, 0.35f, 0.5f);
}
