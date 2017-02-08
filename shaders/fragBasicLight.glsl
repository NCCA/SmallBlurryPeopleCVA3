#version 410 core

#define shadowbuffer 0

in vec2 UV;

uniform sampler2D diffuse;
uniform sampler2D normal;
uniform sampler2D position;
uniform sampler2D shadowDepth;

layout (location = 0) out vec4 fragColour;

uniform vec3 sunDir;
uniform float sunInts;
uniform float moonInts;

uniform mat4 shadowMatrix;

vec2 poissonDisk[4] = vec2[](
        vec2( -0.94201624, -0.39906216 ),
vec2( 0.94558609, -0.76890725 ),
vec2( -0.094184101, -0.92938870 ),
vec2( 0.34495938, 0.29387760 )
);

vec3 moonColour = vec3(0.2, 0.3, 0.4);

void main()
{
    fragColour = vec4(texture(diffuse, UV));
#if shadowbuffer == 0
    if(fragColour.a == 0.0)
        discard;
#endif
    vec3 n = texture(normal, UV).xyz;
    float sunmul = dot(n, sunDir) * sunInts;
    float moonmul = dot(n, -sunDir) * moonInts;
    float mul = sunmul + moonmul;

    mul = clamp(mul, 0.0, 1.0);
    //mul = 1.0;

    vec4 sposition = shadowMatrix * vec4(texture(position, UV).xyz, 1.0);
    float bias = 0.005 * tan( acos( mul ) );
    bias = clamp( bias, 0.0, 0.01);

    for(int i = 0; i < 4; i++)
    {
        vec2 coord = vec2(sposition.xy + poissonDisk[i]/2048.0);
        if((sposition.z - bias) > texture(shadowDepth, coord).r)
            mul -= 0.15;
    }

    mul = max(mul, 0.05);

    //fragColour.xyz = vec3(texture(shadowDepth, sposition.xy).r);

#if shadowbuffer == 1
    fragColour.xyz = vec3(texture(shadowDepth, UV).r);
#else
    fragColour.xyz *= mul;
#endif

#if shadowbuffer == 0
    if(moonmul > 0.0)
        fragColour.xyz *= moonColour;
#endif

    fragColour.a = 1.0;
}
