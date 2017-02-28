#version 410 core

layout( location = 0 ) in vec2 inPosition;
uniform vec2 vResolution;

out vec2 buttonPos;

void main()
{
  vec2 uv;
  uv.x = inPosition.x / vResolution.x;
  uv.y = inPosition.y / vResolution.y;
  buttonPos = vec2(0.5, 0.5);
  gl_Position = vec4(0, 0, -1.0, 1.0);
}
