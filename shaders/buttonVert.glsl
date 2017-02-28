#version 410 core

layout( location = 0 ) in vec2 inPosition;

out vec2 UV;
uniform vec2 vResolution;

void main()
{
  gl_PointSize = 50.0;
  vec2 uv;
  uv.x = inPosition.x;
  uv.y = inPosition.y;
  gl_Position = vec4(uv, 0.0, 1.0);
}
