#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in mat4 instanceModel;

uniform mat4 matLight;
uniform mat4 matModel;
uniform int type;

void main()
{
	if(type == 1)
	{
		gl_Position = matLight * matModel * vec4(position, 1.0f);
	}
	else
	{
		gl_Position = matLight * instanceModel * vec4(position, 1.0f);
	}
}