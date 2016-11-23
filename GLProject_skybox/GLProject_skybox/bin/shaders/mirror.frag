
#version 330 core

varying vec2 fragTexCoord;
uniform sampler2D fragTexture;

out vec4 color;

void main()
{
	//float s = 1.0f - fragTexCoord.x;
	//float t = fragTexCoord.y;
	//const float offset = 1.0f / 300;
	//vec2 offsets[9] = vec2[](
	//	vec2(-offset, offset),
	//	vec2(0, offset),
	//	vec2(offset, offset),
	//	vec2(-offset, 0),
	//	vec2(0, 0),
	//	vec2(offset, 0),
	//	vec2(-offset, -offset),
	//	vec2(0, -offset),
	//	vec2(offset, -offset)
	//);
	//float kernel[9] = float[](
	//	1,2,1,
	//	2,4,2,
	//	1,2,1
	//);
	//vec3 texColor;
	//for(int i = 0; i < 9; ++i)
	//{
	//	texColor += vec3(texture(fragTexture, vec2(s + offsets[i].x, t + offsets[i].y))) * kernel[i] / 16.0f;
	//}
	vec3 texColor = vec3(texture(fragTexture, vec2(1.0f - fragTexCoord.x, fragTexCoord.y)));
	color = vec4( 1- texColor, 1.0f);
}