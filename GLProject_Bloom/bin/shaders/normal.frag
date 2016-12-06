#version 330 core

uniform sampler2D texFBO;
out vec4 fs_outColor;
in vec2 fragTexCoords;

void main()
{
	fs_outColor = texture2D(texFBO, fragTexCoords);
}
