#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 matLightSpace;
uniform mat4 matModel;


void main()
{
	gl_Position = matLightSpace * matModel * vec4(position, 1.0f);
}