
#version 330 core

varying vec2 v_fragTexCoord;

uniform sampler2D u_fragTexture;

out vec4 o_outColor;

void main()
{
	o_outColor = vec4(vec3(texture(u_fragTexture, v_fragTexCoord)),1.0f);
}