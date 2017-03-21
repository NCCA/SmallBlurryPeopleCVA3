#version 410 core

//This shader is just like vertDeferredData, a standard vertex transformation for use within a deferred rendering pipeline.
//However, it is designed for use with a geometry shader, under the assumption that it will perform the MVP multiplication
//in lieu of this.

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;

out vec4 normal_vs;
out vec4 position_vs;
flat out int id_vs;
out vec2 UV_vs;

uniform int ID;

void main()
{
    gl_Position = inPosition;

    normal_vs = vec4(inNormal.xyz, 0.0);
    position_vs = inPosition;
    id_vs = ID;
    UV_vs = inUV;
}
