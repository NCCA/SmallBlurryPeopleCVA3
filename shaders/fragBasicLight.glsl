#version 430 core

in vec2 UV;
in vec3 normal;

layout (location = 0) out vec4 fragColour;

void main()
{
    fragColour = vec4(0.0);
    float mul = dot(normal, vec3(0.0, 1.0, 0.0));
    mul = clamp(mul, 0.0, 1.0);
    fragColour *= mul;
    fragColour = vec4(normal.xyz, 1.0);
}
