#version 410 core

in vec2 UV;

uniform sampler2D diffuse;
uniform sampler2D normal;
uniform sampler2D position;
uniform sampler2D shadowDepth;

layout (location = 0) out vec4 fragColour;

uniform vec3 sunDir;

uniform mat4 shadowMatrix;

vec2 poissonDisk[4] = vec2[](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);

void main()
{
    fragColour = vec4(texture(diffuse, UV));
    if(fragColour.a == 0.0)
        discard;
    vec3 n = texture(normal, UV).xyz;
    float mul = dot(n, sunDir);
    mul = clamp(mul, 0.0, 1.0);
    //mul = 1.0;

    vec4 sposition = shadowMatrix * vec4(texture(position, UV).xyz, 1.0);
    float bias = 0.005 * tan( acos( mul ) );
    bias = clamp( bias, 0.0, 0.01);

    for(int i = 0; i < 4; i++)
    {
        if((sposition.z - bias) > texture(shadowDepth, sposition.xy + poissonDisk[i] / 1024.0).r)
            mul -= 0.1;
    }
    mul = max(mul, 0.1);

    //fragColour.xyz = vec3(texture(shadowDepth, sposition.xy).r);
    //fragColour.xyz = vec3(texture(position, UV));
    fragColour.xyz *= mul;
    fragColour.a = 1.0;
}
