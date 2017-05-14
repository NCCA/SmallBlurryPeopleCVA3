#version 410 core

layout(location = 0) out vec4 outColour;

uniform sampler2D diffuse;
uniform vec2 resolution;

in vec2 UV;

void main()
{
  vec2 new_UV = UV;
  if(resolution.y>resolution.x)
  {
    new_UV.x *= resolution.x/resolution.y;
    new_UV.x += (1-resolution.x/resolution.y)*0.5;
  }
  else
  {
    new_UV.y *= resolution.y/resolution.x;
    new_UV.y += (1-resolution.y/resolution.x)*0.5;
  }
  outColour = texture(diffuse, vec2(new_UV.x, -new_UV.y));
  //outColour.rg = new_UV;
  //outColour.b = 0.0;
  outColour.a = 1.0;
}
