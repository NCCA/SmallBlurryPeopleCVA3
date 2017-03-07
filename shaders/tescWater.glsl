#version 410 core

// define the number of CPs in the output patch
layout (vertices = 4) out;

uniform vec3 gEyeWorldPos;

// attributes of the input CPs
in vec4 position_tc[];
in vec2 uv_tc[];
in vec3 normal_tc[];

// attributes of the output CPs
out vec4 position_te[];
out vec2 uv_te[];
out vec3 normal_te[];

void main()
{
    // Set the control points of the output patch
    uv_te[gl_InvocationID] = position_tc[gl_InvocationID];
    position_te[gl_InvocationID] = uv_tc[gl_InvocationID];
    normal_te[gl_InvocationID] = normal_tc[gl_InvocationID];

    // Calculate the distance from the camera to the three control points
    float EyeToVertexDistance0 = distance(gEyeWorldPos, WorldPos_ES_in[0]);
    float EyeToVertexDistance1 = distance(gEyeWorldPos, WorldPos_ES_in[1]);
    float EyeToVertexDistance2 = distance(gEyeWorldPos, WorldPos_ES_in[2]);

    // Calculate the tessellation levels
    gl_TessLevelOuter[0] = GetTessLevel(EyeToVertexDistance1, EyeToVertexDistance2);
    gl_TessLevelOuter[1] = GetTessLevel(EyeToVertexDistance2, EyeToVertexDistance0);
    gl_TessLevelOuter[2] = GetTessLevel(EyeToVertexDistance0, EyeToVertexDistance1);
    gl_TessLevelInner[0] = gl_TessLevelOuter[2];
}
