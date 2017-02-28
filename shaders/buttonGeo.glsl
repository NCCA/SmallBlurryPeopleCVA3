#version 410 core
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec2 buttonPos[];
in vec2 buttonSize[];

out vec2 fragPos;
out vec2 fragSize;
out vec2 fragUV;

void main()
{
  fragPos = buttonPos[0];
  fragSize = buttonSize[0];

  fragUV = vec2(0.0, 0.0);
  gl_Position = gl_in[0].gl_Position + vec4(0.0, 0.0, 0.0, 0.0);
  EmitVertex();

  fragUV = vec2(0.0, 1.0);
  gl_Position = gl_in[0].gl_Position + vec4(0.0, buttonSize[0].y, 0.0, 0.0);
  EmitVertex();

  fragUV = vec2(1.0, 0.0);
  gl_Position = gl_in[0].gl_Position + vec4(buttonSize[0].x, 0.0, 0.0, 0.0);
  EmitVertex();

  fragUV = vec2(1.0, 1.0);
  gl_Position = gl_in[0].gl_Position + vec4(buttonSize[0].x, buttonSize[0].y, 0.0, 0.0);
  EmitVertex();

  EndPrimitive();
}
