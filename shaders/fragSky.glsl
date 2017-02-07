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
vec3 sunColour = vec3(1.0, 0.9, 0.8);
float lightScatterMul = 1.0;

vec3 airColour = vec3(
            0.18867780436772762, 0.4978442963618773, 0.6616065586417131
            );

int stepCount = 24;

float nmul = 8.0;
float amul = 1.0;

float npow = 8.0;
float apow = 1.0;

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
    float surfaceHeight = 0.65;

    vec3 rayDir = getEyeRay();
    float alpha = dot(rayDir, sunDir);

    float nitrogen = phase(alpha, -0.01);
    float aerosol = phase(alpha, 0.02);
    float spot = smoothstep(0.0, 15.0, phase(alpha, 0.9995));

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

    outColour.a = 1.0;

    /*outColour.rgb = vec3(aerosol);
    outColour.a = 1.0;*/
    //outColour = vec4(1.0, 0.0, 0.0, 1.0);
}
