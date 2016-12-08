#version 330 core

layout (location = 0) out float FragColor;

uniform sampler2D texSSAO;

in vec2 fragTexCoords;

void main()
{
	//获取尺寸
	vec2 size = textureSize(texSSAO, 0);
	//计算单像素尺寸
	size = 1.0 / size;
	for(int x = -2; x < 2; ++x)
	{
		for(int y = -2; y < 2; ++y)
		{
			FragColor += texture2D(texSSAO, fragTexCoords + vec2(x, y)*size).r;
		}
	}
	FragColor = FragColor / 16.0;
}