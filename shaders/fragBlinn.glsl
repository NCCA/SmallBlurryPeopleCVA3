#version 410 core

uniform sampler2D diffuse;

in vec2 UV;
in vec3 normal;

layout (location = 0) out vec4 fragColour;

struct directionalLight
{
    vec3 m_dir;
    vec3 m_colour;
    float m_brightness;
};

directionalLight sun = directionalLight(normalize(vec3(0.0, 1.0, 0.0)), vec3(1.0), 1.0);

void main()
{
    fragColour = texture(diffuse, UV);
    vec3 mul = vec3(dot(normal, sun.m_dir));
    mul = clamp(mul, 0.0, 1.0) * sun.m_colour * sun.m_brightness;
    fragColour.xyz *= mul;
    fragColour.a = 1.0;
}
