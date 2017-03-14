#version 410 core

in vec2 UV;

layout (location = 0) out vec4 fragColour;

uniform sampler2D tex;

void main()
{
    fragColour = vec4(1.0, 0.0, 0.0, 1.0);
    fragColour.xyz = vec3(texture(tex, UV));
    fragColour.a = 1.0;
}
