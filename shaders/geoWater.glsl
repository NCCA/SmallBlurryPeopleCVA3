#version 410 core

layout ( triangles ) in;
layout ( triangle_strip, max_vertices = 4 ) out;

in vec3 normal[];
in vec2 UV[];

out vec3 normalOut[4];
out vec2 UVout[4];

void main()
{
    for(int i = 0; i < gl_in.length(); i++)
    {
        gl_Position = gl_in[i].gl_Position;
        normalOut[i] = normal[i];
        UVout[i] = UV[i];

        EmitVertex();
    }



    EndPrimitive();
}
