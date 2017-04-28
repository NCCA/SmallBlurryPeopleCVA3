#version 410 core

layout(location = 0) out vec4 outColour;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outPosition;
layout(location = 3) out vec4 outDepth;
//layout(location = 3) out int outID;

in vec4 normal_fs;
in vec4 position_fs;
in vec2 UV_fs;
in float time_fs;
in float alpha_fs;

in vec3 tangent_fs;
in vec3 bitangent_fs;

uniform mat4 M;

uniform vec3 camPos;

uniform vec3 directionalLightCol;
uniform vec3 lightDir;

uniform sampler2D t0;
uniform sampler2D t1;
uniform sampler2D t2;

uniform sampler2D normalMap;

void main()
{
  vec3 n = texture(normalMap, UV_fs).xyz;
  n = normalize(n * 2.0 - 1.0);
  n = mat3(tangent_fs, bitangent_fs, normal_fs.xyz) * n;
  n = -n;

  if(time_fs > 0.0 && time_fs < 1.0)
    outColour = mix(texture(t0, UV_fs), texture(t1, UV_fs), vec4(time_fs));
  else if(time_fs > 1.0 && time_fs < 2.0)
    outColour = mix(texture(t1, UV_fs), texture(t2, UV_fs), vec4(time_fs - 1.0));
  else if(time_fs > 2.0)
    outColour = mix(texture(t2, UV_fs), texture(t0, UV_fs), vec4(time_fs - 2.0));

  if(outColour.a == 0.0)
    discard;

  float a = distance(camPos, position_fs.xyz);
  a /= 8.0;
  a = clamp(a, 0.0, 1.0);
  outColour.a *= a;

  outColour.xyz = directionalLightCol;

  float m = dot(n, lightDir);
  m *= 0.5;
  m += 1.0;
  m = clamp(m, 0.0, 1.0);
  outColour.xyz *= m;
  //outColour.a = 1.0;

  outColour.a *= alpha_fs;

  outNormal.xyz = n;
  outPosition = position_fs;
  outDepth = vec4(gl_FragCoord.z / gl_FragCoord.w);
}
