
#version 330 core

int vec2 v_fragTexCoord;

uniform sampler2D u_fragTexture;

out vec4 o_outColor;

void main()
{
	o_outColor = texture2D(u_fragTexture, v_fragTexCoord);
}