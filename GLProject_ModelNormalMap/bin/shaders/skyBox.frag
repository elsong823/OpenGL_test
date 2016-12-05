#version 330 core

uniform samplerCube texSkyBox;
out vec4 outColor;
in vec3 texCoord;

void main()
{
	outColor = texture(texSkyBox, texCoord);
}