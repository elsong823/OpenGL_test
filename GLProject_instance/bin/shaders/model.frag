
#version 330 core

uniform sampler2D tex_diffuse1;
in vec2 fragTexCoord;
out vec4 color;

void main()
{
	color = texture(tex_diffuse1, fragTexCoord);
}
