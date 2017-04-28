#version 410 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in float inScale;
layout(location = 2) in float inTime;
layout(location = 3) in float inAlpha;

out float scale_vs;
out float time_vs;
out float alpha_vs;

void main()
{
  time_vs = inTime;
  scale_vs = inScale;
  alpha_vs = inAlpha;
  gl_Position = vec4(inPosition.xyz, 1.0);
}
