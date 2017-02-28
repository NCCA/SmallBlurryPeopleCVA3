#version 410 core

layout( location = 0 ) in vec2 inPosition;
layout( location = 1 ) in vec2 inSize;

uniform vec2 vResolution;

out vec2 buttonPos;
out vec2 buttonSize;

void main()
{
  // get screenpos assuming button is aligned to top left
  vec2 screenPos;
  vec2 screenSize;
  screenPos.x = inPosition.x / vResolution.x * 2.0 - 1.0;
  screenPos.y = inPosition.y / vResolution.y * 2.0 - 1.0;
  screenPos.y *= -1;

  screenSize.x = inSize.x / vResolution.x * 2;
  screenSize.y = inSize.y / vResolution.y * 2;
  screenSize.y *= -1;

  buttonPos = screenPos;
  buttonSize = screenSize;
  gl_Position = vec4(screenPos, -1.0, 1.0);
}
