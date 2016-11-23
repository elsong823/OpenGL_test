#version 330 core
layout (location = 1) in vec3 position;
void main()
{
	gl_Position = position;
}