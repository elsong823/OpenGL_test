#version 330 core

in vec4 fragColor;
out vec4 color;
	
void main()
{
	float gray = fragColor.r * 0.3333f + fragColor.g * 0.33333f + fragColor.b * 0.33333f;
	color = vec4(gray, gray, gray, 1.0f);
}
