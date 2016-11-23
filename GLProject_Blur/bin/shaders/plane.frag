
#version 330 core

out vec4 color;
uniform sampler2D fragTexture;
in vec2 fragTexCoord;

void main()
{
	color = texture2D(fragTexture, fragTexCoord);
}