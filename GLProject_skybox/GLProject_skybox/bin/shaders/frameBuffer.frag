#version 330 core

uniform sampler2D screenTexture;

varying vec2 fragTexCoords;
out vec4 outColor;



void main()
{
	//取反
	//vec3 color = vec3(texture(screenTexture, fragTexCoords));
	//outColor = vec4(1.0f - color, 1.0f);
	//灰度
	//vec3 color = vec3(texture(screenTexture, fragTexCoords));
	//float average = color.r * 0.2126f + color.g * 0.7152f + color.b * (1.0f - 0.2126f - 0.7152f);
	//outColor = vec4(average, average, average, 1.0f);
	//Kernel effects

	//const float offset = 1.0f / 300;	//精细度
	//float kernel[9] = float[](			//亮度
	//	1, 1, 1,
	//	1, 6, 1,
	//	1, 1, 1
	//);
	//
	//	vec2 offsets[9] = vec2[](
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
	//vec3 col;
	//for(int i = 0; i < 9; ++i)
	//{
	//	col += vec3(texture(screenTexture, fragTexCoords.st + offsets[i])) * kernel[i] / 15;
	//}
	//
	//outColor =  vec4(col, 1.0f);
}