#version 410 core

#define PI 3.14159

layout (quads, cw) in;

in vec4 position_te[];
in vec2 uv_te[];
in vec3 normal_te[];

out vec4 position_fs;
out vec2 uv_fs;
out vec2 tile_uv_fs;
out vec3 normal_fs;

uniform mat4 M;
uniform mat4 MVP;
uniform float iGlobalTime;
uniform sampler2D displacement;
uniform vec2 waterDimensions;

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2, vec2 v3, float u, float v)
{
    vec2 a = mix(v0, v3, u);
    vec2 b = mix(v1, v2, u);
    return normalize(mix(a, b, v));
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2, vec3 v3, float u, float v)
{
    vec3 a = mix(v0, v3, u);
    vec3 b = mix(v1, v2, u);
    return mix(a, b, v);
}

void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    position_fs.xyz = interpolate3D(
                position_te[0].xyz,
            position_te[1].xyz,
            position_te[2].xyz,
            position_te[3].xyz,
            u,
            v
            );

    tile_uv_fs = vec2(u, v);

    normal_fs = interpolate3D(
                normal_te[0],
            normal_te[1],
            normal_te[2],
            normal_te[3],
            u,
            v
            );

    position_fs.z -= texture(displacement, tile_uv_fs).r;
    position_fs.w = 1.0;

    gl_Position = MVP * position_fs;
    position_fs = M * position_fs;

    uv_fs = position_fs.xy / waterDimensions;
}
