
#version 330 core

uniform sampler2D fragTex_1;
uniform sampler2D fragTex_2;
out vec4 color;
in vec2 fragTexCoord;

void main()
{
	color = texture2D(fragTex_1, fragTexCoord) + texture2D(fragTex_2, fragTexCoord);
}