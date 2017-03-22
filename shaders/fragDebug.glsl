#version 410 core

layout(location = 0) out vec4 outColour;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outPosition;
layout(location = 3) out vec4 outDepth;
//layout(location = 3) out int outID;

in vec4 normal_fs;
in vec4 position_fs;

void main()
{
    outColour = vec4(1.0, 0.0, 0.0, 1.0);
    outNormal = normal_fs;
    outPosition = position_fs;
    outDepth = vec4(gl_FragCoord.z / gl_FragCoord.w);
    //outID = inID;
}
