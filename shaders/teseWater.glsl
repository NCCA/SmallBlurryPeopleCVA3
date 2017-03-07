#version 410 core

layout(triangles, equal_spacing, ccw) in;

uniform mat4 MVP;
uniform float iGlobalTime;

in vec4 position_te[];
in vec2 uv_te[];
in vec3 normal_te[];

out vec4 position_fs;
out vec2 uv_fs;
out vec3 normal_fs;

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)
{
    return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
    return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}

void main()
{
    // Interpolate the attributes of the output vertex using the barycentric coordinates
    uv_fs = interpolate2D(uv_te[0], uv_te[1], uv_te[2]);
    normal_fs.xyz = interpolate3D(normal_te[0].xyz, normal_te[1].xyz, normal_te[2].xyz);
    normal_fs = normalize(normal_fs);
    position_fs.xyz = interpolate3D(position_te[0].xyz, position_te[1].xyz, position_te[2].xyz);
    position_fs.w = 1.0;
    position_fs.z += 0.1 * sin(512.0 * position_fs.x + iGlobalTime);

    // Displace the vertex along the normal
    //position_fs += vec4(normal_fs.xyz, 1.0) * vec4(0.0, sin(position_fs.x), 0.0, 0.0);
    gl_Position = MVP * position_fs;
}
