#version 410 core

layout( location = 0 ) in vec2 inPosition;
uniform vec2 vResolution;

out vec2 buttonPos;

void main()
{
  vec2 screenPos;
  screenPos.x = inPosition.x / vResolution.x * 2.0 - 1.0;
  screenPos.y = inPosition.y / vResolution.y * 2.0 - 1.0;
  screenPos.y *= -1;
  screenPos.y -= 0.3;
  buttonPos = screenPos;
  gl_Position = vec4(screenPos, -1.0, 1.0);
}
