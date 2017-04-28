#version 410 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in float inScale;
layout(location = 2) in float inTime;

out float scale_vs;
out float time_vs;

void main()
{
  time_vs = inTime;
  scale_vs = inScale;
  gl_Position = vec4(inPosition.xyz, 1.0);
}
