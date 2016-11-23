#version 330 core

uniform samplerCube cubeTexture;
out vec4 outColor;

varying vec3 fragCoord;
void main()
{
	outColor = texture(cubeTexture, fragCoord);
}