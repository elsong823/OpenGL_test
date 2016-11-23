#version 330 core

uniform sampler2D texDiffuse_1;
out vec4 outColor;
in vec2 fragCoord;

void main()
{
	outColor = texture(texDiffuse_1, fragCoord);
}