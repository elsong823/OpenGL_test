#version 330 core
layout (location = 0) out vec4 FragColor;
in vec2 fragTexCoords;
uniform sampler2D sprite;
uniform bool showDepth;
uniform bool showPos;

void main()
{
	if(showPos)
	{
		FragColor = vec4(texture2D(sprite, fragTexCoords).rgb, 1.0f);
	}
	else if(showDepth)
	{
		float depth = texture2D(sprite, fragTexCoords).a;
		FragColor = vec4(depth, depth, depth, 1.0f);
	}
	else
	{
		//FragColor = texture2D(sprite, fragTexCoords);
		//测试ssao 
		float r = texture2D(sprite, fragTexCoords).r;
		FragColor = vec4(r,r,r,1.0f);
	}
}