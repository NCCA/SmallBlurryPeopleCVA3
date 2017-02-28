#version 410 core
layout(location = 0) out vec4 outColour;

in vec2 fragPos;
in vec2 fragSize;
in vec2 fragUV;
in float fragMousedOver;

void main()
{
  //vec2 buttonUV = (gl_FragCoord.xy - posCoord) / sizeCoord;

  outColour = vec4(fragUV.x, fragUV.y, fragMousedOver, 1.0);
}
