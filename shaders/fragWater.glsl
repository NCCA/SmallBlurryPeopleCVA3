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
    heights[0] = texture(displacement, uv_fs + vec2(0.0, pixelstep.y)).r;
    heights[1] = texture(displacement, uv_fs + vec2(pixelstep.x, 0.0)).r;
    heights[2] = texture(displacement, uv_fs + vec2(0.0, -pixelstep.y)).r;
    heights[3] = texture(displacement, uv_fs + vec2(-pixelstep.x, 0.0)).r;

    vec3 center = vec3(uv_fs.x, texture(displacement, uv_fs).r, uv_fs.y);

    //y/z to be swizzled later.
    vec3 top = vec3((uv_fs + vec2(0.0, pixelstep.y)), heights[0]);
    vec3 right = vec3((uv_fs + vec2(pixelstep.x, 0.0)), heights[1]);
    vec3 bottom = vec3((uv_fs + vec2(0.0, -pixelstep.y)), heights[2]);
    vec3 left = vec3((uv_fs + vec2(pixelstep.x, 0.0)), heights[3]);

    vec3 normals[4];
    normals[0] = normalize( cross( center - top.xzy,       center - right.xzy ) );
    normals[1] = normalize( cross( center - right.xzy,     center - bottom.xzy ) );
    normals[2] = normalize( cross( center - bottom.xzy,    center - left.xzy ) );
    normals[3] = normalize( cross( center - left.xzy,      center - top.xzy ) );

    vec3 nav = normals[0] + normals[1] + normals[2] + normals[3];
    nav /= 4.0;

    vec3 normal = normalize( nav );


    fragColour = vec4(0.1f, 0.2f, 0.35f, 0.5f);
    fragColour = vec4(normal.xyz, 1.0);
    //fragColour = vec4(texture(displacement, uv_fs).r * 16.0);
    fragColour = vec4(uv_fs.x, uv_fs.y, 0.0, 1.0);
    fragColour.a = 1.0;
}
