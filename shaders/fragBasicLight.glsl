
#version 410 core

#define shadowbuffer 0

#define NUM_CASCADES 3

in vec2 UV;

uniform float cascades[NUM_CASCADES + 1];

uniform sampler2D diffuse;
uniform sampler2D normal;
uniform sampler2D position;
uniform sampler2D shadowDepths[NUM_CASCADES];
uniform sampler2D linearDepth;

layout (location = 0) out vec4 fragColour;

uniform vec3 sunDir;
uniform float sunInts;
uniform float moonInts;

uniform mat4 shadowMatrix[NUM_CASCADES];

//UV per pixel 1/viewport
uniform vec2 pixelstep;

vec2 poissonDisk[4] = vec2[](
        vec2( -0.94201624, -0.39906216 ),
vec2( 0.94558609, -0.76890725 ),
vec2( -0.094184101, -0.92938870 ),
vec2( 0.34495938, 0.29387760 )
);

vec3 moonColour = vec3(0.3, 0.6, 0.8);

vec3 fogColour = vec3(0.9, 0.9, 0.94);

uniform vec4 camPos;

float shadowSample(float depth, vec2 smpl, int index)
{  
    if(depth > texture(shadowDepths[index], smpl).r)
        return 1.0f;
    return 0.0;
}

void main()
{
    fragColour = vec4(texture(diffuse, UV));
#if shadowbuffer == 0
    if(fragColour.a == 0.0)
        discard;
#endif
    vec3 n = texture(normal, UV).xyz;
    float sunmul = clamp(dot(n, sunDir), 0.0, 1.0);
    float moonmul = clamp(dot(n, -sunDir), 0.0, 1.0);
    float mul = sunmul + moonmul;
    mul = clamp(mul, 0.0, 1.0);
    float bias = 0.005 * tan( acos( mul ) );
    bias = clamp( bias, 0.0, 0.001);

    sunmul = clamp(dot(n, sunDir), 0.0, 1.0) * sunInts;
    moonmul = clamp(dot(n, -sunDir), 0.0, 1.0) * moonInts;
    mul = sunmul + moonmul;

    int cascadeIndex = -1;
    //TO-DO: Have a better way to compute depth. Maybe write to another buffer.
    float fragDepth = distance(camPos, texture(position, UV));

    if(fragDepth > cascades[0] && fragDepth < cascades[1])
    {
#if shadowbuffer == 2
        fragColour.rgb = vec3(1.0, 0.0, 0.0); return;
#endif
        cascadeIndex = 0;
    }
    else if(fragDepth < cascades[2])
    {
#if shadowbuffer == 2
        fragColour.rgb = vec3(0.0, 1.0, 0.0); return;
#endif
        cascadeIndex = 1;
    }
    else if(fragDepth < cascades[3])
    {
#if shadowbuffer == 2
        fragColour.rgb = vec3(0.0, 0.0, 1.0); return;
#endif
        cascadeIndex = 2;
    }

    if(cascadeIndex != -1)
    {
        vec4 sposition = shadowMatrix[cascadeIndex] * vec4(texture(position, UV).xyz, 1.0);

        float depth = sposition.z - bias;

        float shadow = shadowSample(depth, sposition.xy, cascadeIndex);
        shadow += shadowSample(depth, sposition.xy - pixelstep, cascadeIndex);
        shadow += shadowSample(depth, sposition.xy + vec2(pixelstep.x, -pixelstep.y), cascadeIndex);
        shadow += shadowSample(depth, sposition.xy + vec2(-pixelstep.x, pixelstep.y), cascadeIndex);
        shadow += shadowSample(depth, sposition.xy + pixelstep, cascadeIndex);

        shadow /= 5.0;
        mul -= shadow;
    }

    mul = max(mul, 0.05);

#if shadowbuffer == 1
    fragColour.xyz = vec3(texture(shadowDepths[1], UV).r);
#else
    fragColour.xyz *= mul;
#endif

#if shadowbuffer == 0
    if(moonmul > 0.0)
    {
        fragColour.xyz *= moonColour;
    }
#endif

    float a = clamp(texture(linearDepth, UV).r / 128.0, 0.0, 1.0);
    fragColour.xyz = mix(fragColour.xyz, fogColour, a);
    //fragColour.xyz = vec3(texture(linearDepth, UV).r);

    fragColour.a = 1.0;
}
