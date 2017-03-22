#version 410 core
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec2 buttonPos[];
in vec2 buttonSize[];
in vec2 pixelPos[];
in vec2 pixelSize[];
in float buttonMousedOver[];
flat in int buttonAction[];

out vec2 fragPos;
out vec2 fragSize;
out vec2 fragPixelPos;
out vec2 fragPixelSize;
out vec2 fragUV;
out float fragMousedOver;
flat out int fragAction;

void main()
{
  fragPos = buttonPos[0];
  fragSize = buttonSize[0];
  fragPixelPos = pixelPos[0];
  fragPixelSize = pixelSize[0];
  fragMousedOver = buttonMousedOver[0];
  fragAction = buttonAction[0];

  fragUV = vec2(0.0, 0.0);
  gl_Position = gl_in[0].gl_Position + vec4(0.0, 0.0, 0.0, 0.0);
  EmitVertex();

  fragUV = vec2(0.0, 1.0);
  gl_Position = gl_in[0].gl_Position + vec4(0.0, -buttonSize[0].y, 0.0, 0.0);
  EmitVertex();

  fragUV = vec2(1.0, 0.0);
  gl_Position = gl_in[0].gl_Position + vec4(buttonSize[0].x, 0.0, 0.0, 0.0);
  EmitVertex();

  fragUV = vec2(1.0, 1.0);
  gl_Position = gl_in[0].gl_Position + vec4(buttonSize[0].x, -buttonSize[0].y, 0.0, 0.0);
  EmitVertex();

  EndPrimitive();
}
