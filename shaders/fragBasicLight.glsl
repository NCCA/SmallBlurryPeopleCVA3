#version 430 core

in vec2 UV;
in vec3 normal;

layout (location = 0) out vec4 fragColour;

void main()
{
    fragColour = vec4(1.0);
    float mul = dot(normal, vec3(0.0, 1.0, 0.0));
    mul = clamp(mul, 0.0, 1.0);
    fragColour.xyz *= mul;
    fragColour.a = 1.0;
}
