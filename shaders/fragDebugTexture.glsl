#version 410 core

in vec2 UV;

layout (location = 0) out vec4 fragColour;

uniform sampler2D tex;

void main()
{
    fragColour = vec4(1.0, 0.0, 0.0, 1.0);
    fragColour.xyz = vec3(texture(tex, UV + vec2(0.5))) * 8.0;
    fragColour.a = 1.0;
}
