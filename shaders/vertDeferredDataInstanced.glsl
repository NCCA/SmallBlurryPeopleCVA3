#version 410 core

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;

uniform samplerBuffer transform;

out vec4 normal;
out vec4 position;
flat out int id;
out vec2 UV;

uniform int ID;

uniform mat4 VP;
uniform int offset;

void main()
{
    int x = gl_InstanceID * 4 + offset * 4;
    vec4 c0 = texelFetch( transform, x + 0 );
    vec4 c1 = texelFetch( transform, x + 1 );
    vec4 c2 = texelFetch( transform, x + 2 );
    vec4 c3 = texelFetch( transform, x + 3 );
    mat4 M = mat4( c0, c1, c2, c3 );

    gl_Position = VP * M * inPosition;

    normal = M * vec4(inNormal.xyz, 0.0);
    normal.w = 1.0;
    position = M * inPosition;
    id = ID;
    UV = inUV;
}
