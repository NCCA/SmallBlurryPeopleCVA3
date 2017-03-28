#version 410 core

layout(location = 0) out vec4 outColour;

uniform vec4 colour;
uniform float base;
uniform float m;

in vec4 normal;
in vec4 position;
flat in int inID;
in vec2 UV;

void main()
{
  vec4 c = colour;
  float a = position.y - base;
  a /= 2.0;
  a = clamp(a + m, 0.0, 1.0);
  c.a = mix(1.0, 0.0, a);
  outColour = c;
}
