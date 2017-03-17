#version 410 core

//Adapted from http://codeflow.org/entries/2011/apr/13/advanced-webgl-part-2-sky-rendering/

layout(location = 0) out vec4 outColour;

in vec4 position;
in vec2 UV;

uniform mat4 iP;
uniform mat4 iMV;
uniform vec2 iResolution;
uniform vec3 camPos;

uniform vec3 sunDir;
vec3 sunColour = vec3(1.0);
float lightScatterMul = 1.0;

vec3 airColour = vec3(
            0.18867780436772762, 0.4978442963618773, 0.6616065586417131
            );

int stepCount = 24;

float npow = 1.1;
float apow = 1.0;

uniform float surfaceHeight;

float nmul = 2.1 - surfaceHeight;
float amul = 2.0 - surfaceHeight;

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

vec3 getEyeRay()
{
    vec2 uv = (UV - 0.5) * 2.0;
    vec4 deviceNorm = vec4(uv, 0.0, 1.0);
    //Project to eye space
    vec3 eyeNorm = normalize( (iP * deviceNorm).xyz );
    //Project to world space.
    vec3 worldNorm = normalize( iMV * vec4(eyeNorm, 0.0) ).xyz;
    return worldNorm;
}

float phase(float alpha, float g)
{
    float a = 3.0*(1.0-g*g);
    float b = 2.0*(2.0+g*g);
    float c = 1.0+alpha*alpha;
    float d = pow(1.0+g*g-2.0*g*alpha, 1.5);
    return (a/b)*(c/d);
}

float computeDepth(vec3 position, vec3 dir)
{
    float a = dot(dir, dir);
    float b = 2.0*dot(dir, position);
    float c = dot(position, position)-1.0;
    float det = b*b-4.0*a*c;
    float detSqrt = sqrt(det);
    float q = (-b - detSqrt)/2.0;
    float t1 = c/q;
    return t1;
}

float computeHorizonOcclusion(vec3 position, vec3 dir, float radius)
{
    float u = dot(dir, -position);
    if(u<0.0)
        return 1.0;
    vec3 near = position + u*dir;
    if(length(near) < radius)
        return 0.0;
    else
    {
        vec3 v2 = normalize(near)*radius - position;
        float diff = acos(dot(normalize(v2), dir));
        return smoothstep(0.0, 1.0, pow(diff*2.0, 3.0));
    }
}

vec3 absorb(float dist, vec3 color, float factor)
{
    return color - color * pow( airColour, vec3( factor / dist ));
}

void main()
{
    vec3 rayDir = getEyeRay();
    float alpha = dot(rayDir, sunDir);

    float nitrogen = phase(alpha, -0.01) * nmul;
    float aerosol = phase(alpha, 0.01) * amul;
    float spot = smoothstep(0.0, 15.0, phase(alpha, 0.99));

    vec3 eyePos = vec3(0.0, surfaceHeight, 0.0);
    float eyeDepth = computeDepth(eyePos, rayDir);
    float stepLength = eyeDepth / float(stepCount);

    float eyeOcclusion = computeHorizonOcclusion(
                eyePos, rayDir, surfaceHeight - 0.15
                );

    vec3 nitrogenAccumulation = vec3(0.0, 0.0, 0.0);
    vec3 aerosolAccumulation = vec3(0.0, 0.0, 0.0);

    for(int i = 0; i < stepCount; i++)
    {
        float sdist = stepLength * float(i);
        vec3 position = eyePos + rayDir * sdist;
        float extinction = computeHorizonOcclusion(
                    position, sunDir, surfaceHeight - 0.35
                    );
        float sdepth = computeDepth(position, sunDir);

        float dmul = (cnoise( position * 8.0 ) + 1.0) * 8.0;

        //Avsorb light for the ray. From the sun, to the sample position.
        vec3 influx = absorb(sdepth, sunColour, lightScatterMul) /* extinction*/;

        nitrogenAccumulation += absorb(
                    sdist, airColour * influx, nmul
                    );
        aerosolAccumulation += absorb(
                    sdist, influx, amul
                    );
    }

    nitrogenAccumulation = (
                nitrogenAccumulation *
                eyeOcclusion *
                pow(eyeDepth, npow)
                )/float(stepCount);

    aerosolAccumulation = (
                aerosolAccumulation *
                eyeOcclusion *
                pow(eyeDepth, apow)
                )/float(stepCount);

    //outColour = vec4(nitrogenAccumulation + aerosolAccumulation, 1.0);

    outColour.rgb = vec3(
                spot * aerosolAccumulation +
                aerosol * aerosolAccumulation +
                nitrogen * nitrogenAccumulation
                );

    outColour.rgb = clamp(outColour.rgb, vec3(0.0), vec3(1.0));
    outColour.a = 1.0;

    /*outColour.rgb = vec3(aerosol);
    outColour.a = 1.0;*/
    //outColour = vec4(1.0, 0.0, 0.0, 1.0);
}
