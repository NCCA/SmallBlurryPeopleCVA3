#version 410 core

layout(location = 0) out vec4 outColour;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outPosition;
layout(location = 3) out vec4 outDepth;

uniform vec4 colour;

in vec4 normal;
in vec4 position;
flat in int inID;
in vec2 UV;

void main()
{
    outColour = colour;
    outColour.a = 1.0;
    outNormal = normal;
    outPosition = position;
    outDepth = vec4(gl_FragCoord.z / gl_FragCoord.w);
    //Not setting this to 1 makes for some nasty depth errors. Not sure why, doesn't seem to affect any of the other shaders. TO-DO: Investigate.
    outDepth.a = 1.0;
    //outDepth = vec4(0.5);
}
