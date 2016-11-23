#version 330 core

uniform samplerCube cubeTexture;
varying vec3 cubeTexCoords;

out vec4 color;

void main()
{
	color = texture(cubeTexture, cubeTexCoords);
}