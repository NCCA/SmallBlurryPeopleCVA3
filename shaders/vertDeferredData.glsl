#version 430 core

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;

out vec4 outColour;
out vec4 outNormal;
out vec4 outPosition;
out int outID;

uniform mat4 M;
uniform mat4 MVP;

uniform int ID;

sampler2D diffuse;

void main()
{
    gl_Position = MVP * inPosition;

    outColour = texture(diffuse, inUV);
    outNormal = M * vec4(inNormal.xyz, 0.0);
    outPosition = M * inPosition;
    outID = ID;
}
