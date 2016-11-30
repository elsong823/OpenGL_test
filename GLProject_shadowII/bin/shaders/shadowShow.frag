#version 330 core

uniform sampler2D texShadow;
out vec4 color;
in vec2 fragCoord;
void main()
{
	float depth = texture2D(texShadow, fragCoord).r;
	color = vec4(depth, depth, depth, 1.0f);
}