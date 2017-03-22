#version 410 core

layout ( triangles ) in;
layout ( triangle_strip, max_vertices = 3 ) out;

in vec4 position_vs[];
in vec4 normal_vs[];
in vec2 UV_vs[];

out vec4 position_fs;
out vec4 normal_fs;
out vec2 UV_fs;

uniform vec3 camPos;

uniform mat4 MVP;
uniform mat4 M;

void quad(vec3 _pos, vec3 _dir)
{
    vec3 right = normalize(cross(_dir, vec3(0.0, 1.0, 0.0)));
    vec3 up = normalize(cross(right, _dir));

    //BL
    normal_fs = vec4(-_dir.xyz, 1.0);
    position_fs.xyz = _pos - right;
    position_fs.w = 1.0;
    UV_fs = vec2(0.0, 0.0);
    gl_Position = MVP * position_fs;
    EmitVertex();

    //BR
    normal_fs = vec4(-_dir.xyz, 1.0);
    position_fs.xyz = _pos + right;
    position_fs.w = 1.0;
    UV_fs = vec2(1.0, 0.0);
    gl_Position = MVP * position_fs;
    EmitVertex();

    //TL
    normal_fs = vec4(-_dir.xyz, 1.0);
    position_fs.xyz = _pos - right + up;
    position_fs.w = 1.0;
    UV_fs = vec2(0.0, 1.0);
    gl_Position = MVP * position_fs;
    EmitVertex();

    //TR
    normal_fs = vec4(-_dir.xyz, 1.0);
    position_fs.xyz = _pos + right + up;
    position_fs.w = 1.0;
    UV_fs = vec2(1.0, 1.0);
    gl_Position = MVP * position_fs;
    EmitVertex();

    EndPrimitive();
}

void main()
{
    //Emit 3 quads
    /*vec3 pos = (position_vs[0].xyz + position_vs[1].xyz) / 2.0;
    vec3 dir = normalize(camPos - pos);
    quad(pos, dir);

    pos = (position_vs[1].xyz + position_vs[2].xyz) / 2.0;
    dir = normalize(camPos - pos);
    quad(pos, dir);

    pos = (position_vs[2].xyz + position_vs[0].xyz) / 2.0;
    dir = normalize(camPos - pos);
    quad(pos, dir);*/
    for(int i = 0; i < 3; i++)
    {
        gl_Position = MVP * position_vs[i];
        position_fs = position_vs[i];
        normal_fs = normal_vs[i];
        UV_fs = UV_vs[i];
        EmitVertex();
    }
    EndPrimitive();
}
