#version 410 core

layout(location = 0) out vec4 outColour;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outPosition;
layout(location = 3) out vec4 outDepth;

uniform vec4 colour;
uniform sampler2D diffuse;

in vec4 normal;
in vec4 position;
flat in int id;
in vec2 UV;

void main()
{
	//arbitrary number
	if(UV.y < 0.255)
	{
		outColour = texture(diffuse, vec2(UV.x, -UV.y)) * colour;
	}
	else
	{
		outColour = texture(diffuse, vec2(UV.x, -UV.y));
	}
		outColour.a = 1.0;
		outNormal = normal;
		outPosition = position;
		outDepth = vec4(gl_FragCoord.z / gl_FragCoord.w);
		outDepth.a = 1.0;
}
