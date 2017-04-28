#version 410 core

layout ( points ) in;
layout ( triangle_strip, max_vertices = 4 ) out;

//in vec4 position_vs[];

in float scale_vs[1];
in float time_vs[1];
in float alpha_vs[1];

out vec4 position_fs;
out vec4 normal_fs;
out vec2 UV_fs;
out float time_fs;
out float alpha_fs;

out vec3 tangent_fs;
out vec3 bitangent_fs;

uniform mat4 MVP;

uniform vec3 camPos;

void main()
{
  time_fs = time_vs[0];
  alpha_fs = alpha_vs[0];

  vec3 pos = gl_in[0].gl_Position.xyz;
  vec3 forward = pos - camPos.xyz;
  forward = normalize(forward);

  normal_fs = vec4(forward, 1.0);
  tangent_fs = normalize(cross(normal_fs.xyz, vec3(0,1,0)));
  bitangent_fs = normalize(cross(normal_fs.xyz, tangent_fs));

  vec3 right = normalize(cross(forward, vec3(0,1,0))) * scale_vs[0];
  vec3 up = normalize(cross(right, forward)) * scale_vs[0];
  /*forward = vec3(1.0, 0.0, 0.0);
  up = vec3(0,1,0);
  right = vec3(0,0,1);*/

  //BL
  position_fs.xyz = pos - right - up;
  position_fs.w = 1.0;
  UV_fs = vec2(0.0, 0.0);
  gl_Position = MVP * position_fs;
  EmitVertex();

  //BR
  position_fs.xyz = pos + right - up;
  position_fs.w = 1.0;
  UV_fs = vec2(1.0, 0.0);
  gl_Position = MVP * position_fs;
  EmitVertex();

  //TL
  position_fs.xyz = pos - right + up;
  position_fs.w = 1.0;
  UV_fs = vec2(0.0, 1.0);
  gl_Position = MVP * position_fs;
  EmitVertex();

  //TR
  position_fs.xyz = pos + right + up;
  position_fs.w = 1.0;
  UV_fs = vec2(1.0, 1.0);
  gl_Position = MVP * position_fs;
  EmitVertex();

  EndPrimitive();

}
