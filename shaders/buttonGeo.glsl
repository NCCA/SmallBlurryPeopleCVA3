#version 410 core
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec2 buttonPos[];

void main()
{
  vec2 buttonSize = vec2(0.1,0.1);

  gl_Position = gl_in[0].gl_Position + vec4(0.0, 0.0, 0.0, 0.0);
  EmitVertex();

  gl_Position = gl_in[0].gl_Position + vec4(0.0, buttonSize.y, 0.0, 0.0);
  EmitVertex();

  gl_Position = gl_in[0].gl_Position + vec4(buttonSize.x, 0.0, 0.0, 0.0);
  EmitVertex();

  gl_Position = gl_in[0].gl_Position + vec4(buttonSize.x, buttonSize.y, 0.0, 0.0);
  EmitVertex();

  EndPrimitive();
}
