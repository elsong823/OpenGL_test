#version 330 core

layout (location = 0) out vec4 FragColor;

in vec2 fragTexCoords;
uniform sampler2D texSource;
//是否横向虚化
uniform bool horizontal;
uniform float range;
//虚化权重
uniform float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

void main()
{
	vec2 texSize = textureSize(texSource, 0);
	vec2 offset = 1.0f / texSize;
	vec3 resultPixel = vec3(0.0f) * range;
	if(horizontal)
	{
		for(int i = 1; i < 5; ++i)
		{
			resultPixel += texture2D(texSource, fragTexCoords + vec2(offset.x * i,0.0)).rgb * weight[i];
			resultPixel += texture2D(texSource, fragTexCoords - vec2(offset.x * i,0.0)).rgb * weight[i];
		}
		resultPixel += texture2D(texSource, fragTexCoords).rgb * weight[0];
	}
	else
	{
		for(int i = 1; i < 5; ++i)
		{
			resultPixel += texture2D(texSource, fragTexCoords + vec2(0.0, offset.y * i)).rgb * weight[i];
			resultPixel += texture2D(texSource, fragTexCoords - vec2(0.0, offset.y * i)).rgb * weight[i];
		}
		resultPixel += texture2D(texSource, fragTexCoords).rgb * weight[0];
	}
	FragColor = vec4(resultPixel, 1.0);
}
