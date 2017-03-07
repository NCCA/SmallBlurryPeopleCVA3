#version 410 core

layout(location = 0) out vec4 outPosition;

in vec4 position;
uniform vec2 dimensions;

void main()
{
	float x_pos = position.x/dimensions.x;
	float z_pos = position.z/dimensions.y;
	outPosition = vec4(x_pos, position.y, z_pos, 1.0);
	//outID = inID;
}
