#version 330 core

uniform sampler2D texDiff;
out vec4 color;
in vec2 fragCoords;

void main()
{
	float depth = texture2D(texDiff, fragCoords).r;
	color = vec4(vec3(depth),1.0f);
}