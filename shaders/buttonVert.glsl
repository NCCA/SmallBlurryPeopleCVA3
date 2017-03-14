#version 410 core

layout( location = 0 ) in vec2 inPosition;
layout( location = 1 ) in vec2 inSize;
layout( location = 2 ) in int inId;

uniform vec2 vResolution;
uniform int mouseOver;

out vec2 buttonPos;
out vec2 buttonSize;
out float buttonMousedOver; // 1 for true, 0 for false

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

  buttonPos = screenPos;
  buttonSize = screenSize;

  if(inId == mouseOver) buttonMousedOver = 1.0;
  else buttonMousedOver = 0.0;

  gl_Position = vec4(screenPos, -1.0, 1.0);
}
