#version 410 core

//Adapted from http://codeflow.org/entries/2011/apr/13/advanced-webgl-part-2-sky-rendering/

layout(location = 0) out vec4 outColour;

in vec4 position;
in vec2 UV;

uniform mat4 iP;
uniform mat4 iMV;

uniform vec2 iResolution;

uniform vec3 camPos;

vec3 sunDir = vec3(0.0, 0.0, 1.0);

vec3 airColour = vec3(
    0.18867780436772762, 0.4978442963618773, 0.6616065586417131
);

int stepCount = 16;
float stepLength = 0.01;

vec3 getEyeRay()
{
    vec2 uv = (UV - 0.5) * 2.0;
    vec4 device_normal = vec4(uv, 0.0, 1.0);
    //Project to eye space
    vec3 eye_normal = normalize( (iP * device_normal).xyz );
    //Project to world space.
    vec3 world_normal = normalize( iMV * vec4(eye_normal, 0.0) ).xyz;
    return world_normal;
}

float phase(float alpha, float g)
{
    float a = 3.0*(1.0-g*g);
    float b = 2.0*(2.0+g*g);
    float c = 1.0+alpha*alpha;
    float d = pow(1.0+g*g-2.0*g*alpha, 1.5);
    return (a/b)*(c/d);
}

void main()
{
    vec3 rayDir = getEyeRay();
    float alpha = dot(rayDir, sunDir);

    float nitrogen = phase(alpha, -0.01);
    float aerosol = phase(alpha, 0.02);
    //float spot = smoothstep(0.0, 15.0, phase(alpha, 0.9995))*spot_brightness;

    vec3 nitrogenAccumulation = vec3(0.0, 0.0, 0.0);
    vec3 aerosolAccumulation = vec3(0.0, 0.0, 0.0);

    for(int i = 0; i < stepCount; i++)
    {
        float sdist = stepLength * float( i );
        vec3 position = camPos + rayDir * sdist;

        nitrogenAccumulation += airColour * alpha;
    }

    /*nitrogenAccumulation /= stepCount;
    aerosolAccumulation /= stepCount;*/

    outColour = vec4(nitrogenAccumulation + aerosolAccumulation, 1.0);
    //outID = inID;
    outColour = vec4(1.0, 0.0, 0.0, 1.0);
}
