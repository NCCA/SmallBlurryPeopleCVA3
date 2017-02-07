#version 410 core

in vec2 UV;

uniform isampler2D diffuse;
uniform sampler2D normal;
uniform sampler2D position;
//uniform sampler2D id;

layout (location = 0) out vec4 fragColour;

void main()
{
    int r = 2;
    fragColour = vec4(r);
    fragColour = vec4(texture(diffuse, UV).r);
    /*vec3 n = texture(normal, UV).xyz;
    float mul = dot(n, vec3(0.0, 1.0, 0.0));
    mul = clamp(mul, 0.0, 1.0);
    fragColour.xyz *= mul;*/
    fragColour.a = 1.0;
}
