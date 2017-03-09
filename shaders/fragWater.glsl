#version 410 core

layout (location = 0) out vec4 fragColour;

in vec4 position_fs;
in vec3 normal_fs;
in vec2 uv_fs;

uniform sampler2D displacement;
uniform vec2 pixelstep;

void main()
{
    //Calculate normal from displacement.
    float heights[4];
    heights[0] = textureOffset(displacement, uv_fs, ivec2(0, 1)).r;
    heights[1] = textureOffset(displacement, uv_fs, ivec2(1, 0)).r;
    heights[2] = textureOffset(displacement, uv_fs, ivec2(0, -1)).r;
    heights[3] = textureOffset(displacement, uv_fs, ivec2(-1, 0)).r;

    vec3 center = vec3(uv_fs.xy, texture(displacement, uv_fs).r);

    //y/z to be swizzled later.
    vec3 top = vec3((uv_fs + vec2(0.0, pixelstep.y)), heights[0]) - center;
    vec3 right = vec3((uv_fs + vec2(pixelstep.x, 0.0)), heights[1]) - center;
    vec3 bottom = vec3((uv_fs + vec2(0.0, -pixelstep.y)), heights[2]) - center;
    vec3 left = vec3((uv_fs + vec2(pixelstep.x, 0.0)), heights[3]) - center;

    bottom.x = -bottom.x;
    left.x = -left.x;
    //right.z = -right.z;

    vec3 normals[4];
    normals[0] = normalize( cross( top.xzy,       right.xzy ) );
    normals[1] = normalize( cross( right.xzy,     bottom.xzy ) );
    normals[2] = normalize( cross( bottom.xzy,    left.xzy ) );
    normals[3] = normalize( cross( left.xzy,      top.xzy ) );

    vec3 nav = normals[0] + normals[1] + normals[2] + normals[3];
    nav /= 4.0;

    vec3 normal = normalize( nav );
    fragColour = vec4(0.1f, 0.2f, 0.35f, 0.5f);
    fragColour = vec4(normal, 1.0);
    //fragColour = vec4(texture(displacement, uv_fs).r * 16.0);
    //fragColour = vec4(uv_fs.x, uv_fs.y, 0.0, 1.0);
    fragColour.a = 1.0;
}
