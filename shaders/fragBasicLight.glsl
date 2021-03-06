#version 410 core

#define shadowbuffer 0
#define NUM_CASCADES 3

#define FOG_DIVISOR 256.0

in vec2 UV;

uniform bool drawGodRays;

uniform float cascades[NUM_CASCADES + 1];

uniform sampler2D diffuse;
uniform sampler2D normal;
uniform sampler2D position;
uniform sampler2D shadowDepths[NUM_CASCADES];
uniform sampler2D linearDepth;
uniform sampler2D rayDir;

uniform vec2 gridDimensions;

layout (location = 0) out vec4 fragColour;

struct light
{
  vec4 pos;
  vec3 col;
  float lum;
};
layout( std140 ) uniform lightBuffer
{
  light buf [512];
} lbuf;
uniform int lbufLen;

uniform vec3 sunDir;
uniform float sunInts;
uniform float moonInts;
uniform float waterLevel;
uniform vec3 directionalLightCol;
uniform float iGlobalTime;

uniform mat4 shadowMatrix[NUM_CASCADES];

vec3 moonColour = vec3(0.3, 0.6, 0.8);

uniform vec4 camPos;

vec3 getEyeRay()
{
  return texture(rayDir, UV).xyz;
}

//Noise functions from https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83
//  Classic Perlin 3D Noise
//  by Stefan Gustavson
//
vec4 permute(vec4 x){return mod(((x*34.0)+1.0)*x, 289.0);}
vec4 taylorInvSqrt(vec4 r){return 1.79284291400159 - 0.85373472095314 * r;}
vec3 fade(vec3 t) {return t*t*t*(t*(t*6.0-15.0)+10.0);}

float cnoise(vec3 P){
  vec3 Pi0 = floor(P); // Integer part for indexing
  vec3 Pi1 = Pi0 + vec3(1.0); // Integer part + 1
  Pi0 = mod(Pi0, 289.0);
  Pi1 = mod(Pi1, 289.0);
  vec3 Pf0 = fract(P); // Fractional part for interpolation
  vec3 Pf1 = Pf0 - vec3(1.0); // Fractional part - 1.0
  vec4 ix = vec4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
  vec4 iy = vec4(Pi0.yy, Pi1.yy);
  vec4 iz0 = Pi0.zzzz;
  vec4 iz1 = Pi1.zzzz;

  vec4 ixy = permute(permute(ix) + iy);
  vec4 ixy0 = permute(ixy + iz0);
  vec4 ixy1 = permute(ixy + iz1);

  vec4 gx0 = ixy0 / 7.0;
  vec4 gy0 = fract(floor(gx0) / 7.0) - 0.5;
  gx0 = fract(gx0);
  vec4 gz0 = vec4(0.5) - abs(gx0) - abs(gy0);
  vec4 sz0 = step(gz0, vec4(0.0));
  gx0 -= sz0 * (step(0.0, gx0) - 0.5);
  gy0 -= sz0 * (step(0.0, gy0) - 0.5);

  vec4 gx1 = ixy1 / 7.0;
  vec4 gy1 = fract(floor(gx1) / 7.0) - 0.5;
  gx1 = fract(gx1);
  vec4 gz1 = vec4(0.5) - abs(gx1) - abs(gy1);
  vec4 sz1 = step(gz1, vec4(0.0));
  gx1 -= sz1 * (step(0.0, gx1) - 0.5);
  gy1 -= sz1 * (step(0.0, gy1) - 0.5);

  vec3 g000 = vec3(gx0.x,gy0.x,gz0.x);
  vec3 g100 = vec3(gx0.y,gy0.y,gz0.y);
  vec3 g010 = vec3(gx0.z,gy0.z,gz0.z);
  vec3 g110 = vec3(gx0.w,gy0.w,gz0.w);
  vec3 g001 = vec3(gx1.x,gy1.x,gz1.x);
  vec3 g101 = vec3(gx1.y,gy1.y,gz1.y);
  vec3 g011 = vec3(gx1.z,gy1.z,gz1.z);
  vec3 g111 = vec3(gx1.w,gy1.w,gz1.w);

  vec4 norm0 = taylorInvSqrt(vec4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
  g000 *= norm0.x;
  g010 *= norm0.y;
  g100 *= norm0.z;
  g110 *= norm0.w;
  vec4 norm1 = taylorInvSqrt(vec4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
  g001 *= norm1.x;
  g011 *= norm1.y;
  g101 *= norm1.z;
  g111 *= norm1.w;

  float n000 = dot(g000, Pf0);
  float n100 = dot(g100, vec3(Pf1.x, Pf0.yz));
  float n010 = dot(g010, vec3(Pf0.x, Pf1.y, Pf0.z));
  float n110 = dot(g110, vec3(Pf1.xy, Pf0.z));
  float n001 = dot(g001, vec3(Pf0.xy, Pf1.z));
  float n101 = dot(g101, vec3(Pf1.x, Pf0.y, Pf1.z));
  float n011 = dot(g011, vec3(Pf0.x, Pf1.yz));
  float n111 = dot(g111, Pf1);

  vec3 fade_xyz = fade(Pf0);
  vec4 n_z = mix(vec4(n000, n100, n010, n110), vec4(n001, n101, n011, n111), fade_xyz.z);
  vec2 n_yz = mix(n_z.xy, n_z.zw, fade_xyz.y);
  float n_xyz = mix(n_yz.x, n_yz.y, fade_xyz.x);
  return 2.2 * n_xyz;
}

float shadowSample(float depth, vec2 smpl, ivec2 offset, int index)
{
  if(depth > textureOffset(shadowDepths[index], smpl, offset).r)
    return 1.0;
  return 0.0;
}

float distanceSquared(vec3 _a, vec3 _b)
{
  return (_b.x - _a.x) * (_b.x - _a.x) +
      (_b.y - _a.y) * (_b.y - _a.y) +
      (_b.z - _a.z) * (_b.z - _a.z);
}

vec3 basicLight(vec4 fragPos, vec3 fragNormal, int lightIndex)
{
  vec4 lp = lbuf.buf[ lightIndex ].pos;
  float d = distanceSquared(fragPos.xyz, lp.xyz);

  //Light cutoff threshold
  if(lbuf.buf[ lightIndex ].lum / d < 0.001)
    return vec3(0.0);

  vec4 lv = lp - fragPos;
  lv = normalize(lv);
  float mul = dot( lv, vec4(fragNormal.xyz, 0.0) );
  mul = clamp(mul, 0.0, 1.0);

  vec3 base = lbuf.buf[ lightIndex ].col.xyz;
  d = distance(lp, fragPos);
  d *= d;
  return (mul * lbuf.buf[ lightIndex ].lum * base) / (1.0 + d);
}

void main()
{
  fragColour = vec4(texture(diffuse, UV));
  vec4 fragPos = texture(position, UV);
  vec3 fragNorm = texture(normal, UV).xyz;
  float fragDepth = texture(linearDepth, UV).r;

#if shadowbuffer == 0
  if(fragColour.a == 0.0)
    discard;
#endif
  float sunmul = clamp(dot(fragNorm, sunDir), 0.0, 1.0);
  float moonmul = clamp(dot(fragNorm, -sunDir), 0.0, 1.0);
  float mul = sunmul + moonmul;
  mul = clamp(mul, 0.0, 1.0);
  float bias = 0.005 * tan( acos( mul ) );
  bias = clamp( bias, 0.0, 0.001);

  sunmul = clamp(dot(fragNorm, sunDir), 0.0, 1.0) * sunInts;
  moonmul = clamp(dot(fragNorm, -sunDir), 0.0, 1.0) * moonInts;
  mul = sunmul + moonmul;

  int cascadeIndex = -1;

  if(fragDepth > cascades[0] && fragDepth < cascades[1])
    cascadeIndex = 0;
  else if(fragDepth < cascades[2])
    cascadeIndex = 1;
  else if(fragDepth < cascades[3])
    cascadeIndex = 2;

  if(cascadeIndex != -1)
  {
    vec4 sposition = shadowMatrix[cascadeIndex] * vec4(fragPos.xyz, 1.0);

    float depth = sposition.z - bias;

    float shadow = shadowSample(depth, sposition.xy, ivec2(0, 0), cascadeIndex);
    shadow += shadowSample(depth, sposition.xy, ivec2(0, 1), cascadeIndex);
    shadow += shadowSample(depth, sposition.xy, ivec2(1, 0), cascadeIndex);
    shadow += shadowSample(depth, sposition.xy, ivec2(0, -1), cascadeIndex);
    shadow += shadowSample(depth, sposition.xy, ivec2(-1, 0), cascadeIndex);

    shadow /= 5.0;
    mul -= shadow;
  }

#if shadowbuffer == 0
  //GODRAYS
  if(drawGodRays)
  {
    //Step length is now variable, it shortens when in shadow and lengthens when not.
    //Edit, this did NOT work
    float stepLength = 0.05;
    float rayLen = distance(camPos, fragPos);
    float curLen = 0.0;
    vec3 rayDir = getEyeRay();
    float lum = 0.0;
    int count = 0;
    while(curLen < rayLen && curLen < 24.0)
    {
      int cascadeIndex = -1;
      if(curLen > cascades[0] && curLen < cascades[1])
        cascadeIndex = 0;
      else if(curLen < cascades[2])
        cascadeIndex = 1;
      else if(curLen < cascades[3])
        cascadeIndex = 2;

      vec3 rayPos = camPos.xyz + curLen * rayDir;
      vec4 sposition = shadowMatrix[cascadeIndex] * vec4(rayPos.xyz, 1.0);
      float depth = sposition.z - bias;

      float shadow = shadowSample(depth, sposition.xy, ivec2(0, 0), cascadeIndex);
      shadow += shadowSample(depth, sposition.xy, ivec2(0, 1), cascadeIndex);
      shadow += shadowSample(depth, sposition.xy, ivec2(1, 0), cascadeIndex);
      shadow += shadowSample(depth, sposition.xy, ivec2(0, -1), cascadeIndex);
      shadow += shadowSample(depth, sposition.xy, ivec2(-1, 0), cascadeIndex);
      shadow /= 5.0;

      lum += 1.15 - 1.0 * shadow * curLen;

      count++;
      curLen += stepLength;
    }
    lum /= count;
    lum = max(0.15, lum);
    mul *= lum;
    /*fragColour = vec4(lum,lum,lum,1.0);
    return;*/
  }

  mul = max(mul, 0.35 / (sunmul + moonmul + 1.0));
  fragColour.xyz *= mul;
  fragColour.xyz *= directionalLightCol;

  for(int i = 0; i < lbufLen; ++i)
  {
    fragColour.xyz += basicLight(fragPos, fragNorm, i);
  }
#endif

  //Fog
  float a = clamp(texture(linearDepth, UV).r / FOG_DIVISOR, 0.0, 1.0);
  fragColour.xyz = mix(fragColour.xyz, 0.8 * directionalLightCol * (clamp(sunInts, 0.0, 1.0) + clamp(moonInts, 0.0, 1.0)), a);
  //fragColour.xyz = vec3(texture(linearDepth, UV).r);

  if(fragPos.x > 0.0 && fragPos.x < gridDimensions.x &&
     fragPos.z > 0.0 && fragPos.z < gridDimensions.y
     )
  {
    if(camPos.y < waterLevel)
      fragColour.xyz *= vec3(0.1, 0.2, 0.35) * directionalLightCol;

    float d = clamp(waterLevel - fragPos.y, 0.0, 1.0);
    fragColour.xyz = mix(fragColour.xyz, fragColour.xyz * directionalLightCol * vec3(0.1, 0.2, 0.35), d);

    float wigglyLightMul = (1.1 - d) * (d * d);
    vec3 wiggles = vec3(1.0) - abs(cnoise(fragPos.xyz * 8.0 + vec3(0.0, iGlobalTime * 0.25, 0.0)));
    wiggles = pow(wiggles, vec3(4.0));
    wiggles = max(wiggles, 0.0);
    fragColour.xyz += wigglyLightMul * wiggles * directionalLightCol;
  }

#if shadowbuffer == 1
  fragColour.xyz = vec3(texture(shadowDepths[0], UV).r);
#endif

  //fragColour.xyz = vec3(fragPos.xyz) / 16.0;

  fragColour.a = 1.0;
}
