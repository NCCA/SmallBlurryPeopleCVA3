#version 410 core

// define the number of CPs in the output patch
layout (vertices = 4) out;

uniform mat4 M;

uniform vec3 gEyeWorldPos;
uniform int maxTessLevel;

// attributes of the input CPs
in vec4 position_tc[];
in vec2 uv_tc[];
in vec3 normal_tc[];

// attributes of the output CPs
out vec4 position_te[];
out vec2 uv_te[];
out vec3 normal_te[];

float GetTessLevel(float Distance0, float Distance1)
{
    float AvgDistance = (Distance0 + Distance1) / 2.0;

    int t = 32;//min(maxTessLevel, 256);

    if (AvgDistance <= 8.0)
    {
        return t;
    }
    else if (AvgDistance <= 32.0)
    {
        return t / 2;
    }
    else
    {
       return t / 8;
    }
}

void main()
{
    // Set the control points of the output patch
    position_te[gl_InvocationID] = position_tc[gl_InvocationID];
    uv_te[gl_InvocationID] = uv_tc[gl_InvocationID];
    normal_te[gl_InvocationID] = normal_tc[gl_InvocationID];

    if(gl_InvocationID == 0)
    {
        // Calculate the distance from the camera to the three control points
        float EyeToVertexDistance0 = distance(gEyeWorldPos, (M * position_te[0]).xyz);
        float EyeToVertexDistance1 = distance(gEyeWorldPos, (M * position_te[1]).xyz);
        float EyeToVertexDistance2 = distance(gEyeWorldPos, (M * position_te[2]).xyz);
        float EyeToVertexDistance3 = distance(gEyeWorldPos, (M * position_te[3]).xyz);

        // Calculate the tessellation levels
        gl_TessLevelOuter[0] = GetTessLevel(EyeToVertexDistance0, EyeToVertexDistance1);
        gl_TessLevelOuter[1] = GetTessLevel(EyeToVertexDistance0, EyeToVertexDistance3);
        gl_TessLevelOuter[2] = GetTessLevel(EyeToVertexDistance2, EyeToVertexDistance3);
        gl_TessLevelOuter[3] = GetTessLevel(EyeToVertexDistance1, EyeToVertexDistance2);

        gl_TessLevelInner[0] = gl_TessLevelOuter[2];
        gl_TessLevelInner[1] = gl_TessLevelInner[0];
    }
}
