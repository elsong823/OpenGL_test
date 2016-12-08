#version 330 core

layout (location = 0) out vec4 FragColor;

in vec2 texCoords;
uniform sampler2D texText;
uniform vec3 textColor;

void main()
{
	vec4 sampled = vec4(1.0f, 1.0, 1.0, texture2D(texText, texCoords).r);
	FragColor = vec4(textColor, 1.0) * sampled;
}