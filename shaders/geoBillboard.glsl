#version 410 core

layout ( points ) in;
layout ( triangle_strip, max_vertices = 4 ) out;

//in vec4 position_vs[];

in float scale_vs[1];
in float time_vs[1];

out vec4 position_fs;
out vec4 normal_fs;
out vec2 UV_fs;
out float time_fs;

out mat3 TBN;

uniform mat4 MVP;

uniform vec3 camPos;

void main()
{
  //If you don't initialize TBN here is never assigned
  TBN = mat3(0.0);

  time_fs = time_vs[0];

  vec3 pos = gl_in[0].gl_Position.xyz;
  vec3 forward = pos - camPos.xyz;
  normalize(forward);
  vec3 right = normalize(cross(forward, vec3(0,1,0))) * scale_vs[0];
  vec3 up = normalize(cross(right, forward)) * scale_vs[0];

  normal_fs = vec4(forward, 1.0);

  vec3 p[3];
  vec2 u[3];
  p[0] = pos - right - up;
  u[0] = vec2(0.0, 0.0);
  p[1] = pos + right - up;
  u[1] = vec2(1.0, 0.0);
  p[2] = pos - right + up;
  u[2] = vec2(0.0, 1.0);

  //https://learnopengl.com/#!Advanced-Lighting/Normal-Mapping
  vec3 edge0 = p[1] - p[0];
  vec3 edge1 = p[2] - p[0];
  vec2 deltaUV0 = u[1] - u[0];
  vec2 deltaUV1 = u[2] - u[0];

  float f = 1.0f / (deltaUV0.x * deltaUV1.y - deltaUV1.x * deltaUV0.y);

  vec3 tangent;
  vec3 bitangent;

  tangent = f * (edge0 * deltaUV1.y - edge1 * deltaUV0.y);
  /*tangent.x = f * (deltaUV1.y * edge0.x - deltaUV0.y * edge1.x);
  tangent.y = f * (deltaUV1.y * edge0.y - deltaUV0.y * edge1.y);
  tangent.z = f * (deltaUV1.y * edge0.z - deltaUV0.y * edge1.z);*/
  tangent = normalize(tangent);

  bitangent = f * (edge1 * deltaUV0.x - edge0 * deltaUV1.x);
  /*bitangent.x = f * (-deltaUV1.x * edge0.x + deltaUV0.x * edge1.x);
  bitangent.y = f * (-deltaUV1.x * edge0.y + deltaUV0.x * edge1.y);
  bitangent.z = f * (-deltaUV1.x * edge0.z + deltaUV0.x * edge1.z);*/
  bitangent = normalize(bitangent);

  TBN = mat3(tangent, bitangent, normal_fs);
  TBN = transpose(TBN);


  //BL
  position_fs.xyz = p[0];
  position_fs.w = 1.0;
  UV_fs = u[0];
  gl_Position = MVP * position_fs;
  EmitVertex();

  //BR
  position_fs.xyz = p[1];
  position_fs.w = 1.0;
  UV_fs = u[1];
  gl_Position = MVP * position_fs;
  EmitVertex();

  //TL
  position_fs.xyz = p[2];
  position_fs.w = 1.0;
  UV_fs = u[2];
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
