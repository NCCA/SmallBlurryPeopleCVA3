#version 410 core

#define STEP_DIST 0.05

layout (location = 0) out vec4 fragColour;

in vec4 position_fs;
in vec3 normal_fs;
in vec2 uv_fs;

uniform sampler2D displacement;
uniform sampler2D terrainPos;
uniform vec2 pixelstep;
uniform vec3 lightDir;
uniform vec3 camPos;
uniform vec2 viewport;
uniform vec3 directionalLightCol;

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

void main()
{
    vec2 UV = gl_FragCoord.xy / viewport;

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

    float mul = dot(normal, lightDir);
    mul += 1.0;
    mul /= 2.0;

    vec3 eyeVec = normalize(camPos - position_fs.xyz);
    //vec3 halfVec = normalize(lightDir - eyeVec);
    vec3 reflection = reflect(lightDir, normal);
    //float smul = clamp(dot(eyeVec, reflection), 0.0, 1.0);
    float smul = max( dot(eyeVec, reflection), 0.0 );
    smul = pow( smul, 8.0 );

    fragColour.xyz = vec3(0.1, 0.2, 0.35) * directionalLightCol * mul;
    fragColour.xyz += directionalLightCol * smul;
    fragColour.a = 0.5 * (-abs(mul) + 1.0) + 0.5;
    float d = distance(position_fs, texture(terrainPos, UV));
    fragColour.a += d / 8.0;

    float waterDepth = max(position_fs.y - texture(terrainPos, UV).y, 0.001);
    float dmul = clamp(cnoise(position_fs.xyz * 32.0), 0.25, 1.0);
    dmul *= 1.0 / (waterDepth * 32.0);

    fragColour.xyz = mix(fragColour.xyz, directionalLightCol, clamp(dmul, 0.0, 1.0));

    //fragColour.xyz = vec3(distance(position_fs, texture(terrainPos, UV)) / 4.0);
    //fragColour.xyz = position_fs.xyz;
    //fragColour.xyz = normal;
    //fragColour.a = 1.0;

    fragColour.a = clamp(fragColour.a, 0.0, 1.0);
}
