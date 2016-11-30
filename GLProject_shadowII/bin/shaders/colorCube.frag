#version 330 core

uniform vec3 inColor;
out vec4 outColor;

void main()
{
	outColor = vec4(inColor, 1.0f);
}