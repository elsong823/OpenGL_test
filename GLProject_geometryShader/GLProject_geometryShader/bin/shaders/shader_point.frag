#version 330 core

out vec4 color;
in vec3 heheColor;

void main()
{
	color = vec4(heheColor, 1.0f);
}