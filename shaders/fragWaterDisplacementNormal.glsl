#version 410 core

#define PI 3.14159

layout(location = 0) out vec4 outNormal;

in vec4 normal;
in vec4 position;
in vec2 UV;

uniform sampler2D displacement;
uniform vec2 pixelstep;

void main()
{
    //Calculate normal from displacement.
    float heights[4];
    heights[0] = textureOffset(displacement, UV, ivec2(0, 1)).r;
    heights[1] = textureOffset(displacement, UV, ivec2(1, 0)).r;
    heights[2] = textureOffset(displacement, UV, ivec2(0, -1)).r;
    heights[3] = textureOffset(displacement, UV, ivec2(-1, 0)).r;

    vec3 center = vec3(UV.xy, texture(displacement, UV).r);

    //y/z to be swizzled later.
    vec3 top = vec3((UV + vec2(0.0, pixelstep.y)), heights[0]) - center;
    vec3 right = vec3((UV + vec2(pixelstep.x, 0.0)), heights[1]) - center;
    vec3 bottom = vec3((UV + vec2(0.0, -pixelstep.y)), heights[2]) - center;
    vec3 left = vec3((UV + vec2(pixelstep.x, 0.0)), heights[3]) - center;

    bottom.x = -bottom.x;
    left.x = -left.x;

    vec3 normals[4];
    normals[0] = normalize( cross( top.xzy,       right.xzy ) );
    normals[1] = normalize( cross( right.xzy,     bottom.xzy ) );
    normals[2] = normalize( cross( bottom.xzy,    left.xzy ) );
    normals[3] = normalize( cross( left.xzy,      top.xzy ) );

    vec3 nav = normals[0] + normals[1] + normals[2] + normals[3];
    nav /= 4.0;

    vec3 normal = normalize( nav );

    outNormal = vec4(normal.xyz, 1.0);
}
