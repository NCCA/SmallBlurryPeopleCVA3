#version 410 core

layout(location = 0) out vec4 outColour;

in vec4 normal;
in vec4 position;
in vec2 UV;

uniform sampler2D displacement;

void main()
{
  /*vec2 uvx = vec2(fract(UV.x + 0.5), UV.y);
  vec2 uvy = vec2(UV.x, fract(UV.y + 0.5));

  vec4 col = texture(displacement, UV);
  vec4 colx = texture(displacement, uvx);
  vec4 coly = texture(displacement, uvy);

  //outColour = mix(col, colx, mixx);
  //outColour = mix(coly, outColour, mixy);
  outColour = col;*/

  vec2 uvo = UV + vec2(0.5);

  vec4 col = texture(displacement, UV);
  vec4 colo = texture(displacement, uvo);

  /*float mx = abs(dot(UV - vec2(0.5,0.5), vec2(1.0,1.0)));
  float my = abs(dot(UV - vec2(0.5,0.5), vec2(-1.0,1.0)));*/
  float mx = abs(UV.x - 0.5);
  float my = abs(UV.y - 0.5);

  outColour = mix(col, colo, mx+my);
}
