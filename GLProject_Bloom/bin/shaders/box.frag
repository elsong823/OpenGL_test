#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

struct stLight
{
	vec3 lightPos;
	vec3 lightColor;
};

uniform sampler2D texDiffuse;
uniform vec3 viewPos;
uniform stLight lights[4];

in VS_OUT
{
	vec2 texCoords;
	vec3 fragPos;
	vec3 normal;
}fs_in;

void main()
{
	vec3 resultColor = vec3(0.0f);
	vec3 pixColor = texture2D(texDiffuse, fs_in.texCoords).rgb;
	vec3 normal = normalize(fs_in.normal);
	for(int i = 0; i < 4; ++i)
	{
		vec3 lightDir = normalize(fs_in.fragPos - lights[i].lightPos);
		float distance = length(fs_in.fragPos - lights[i].lightPos);
		float attenuation = 1.0f / (distance * distance);
		float ambient = 0.0f;
		resultColor += lights[i].lightColor * ambient * pixColor;
		float diff = max(dot(-lightDir, normal), 0.0f);
		resultColor += lights[i].lightColor * diff * attenuation * pixColor;
	}
	FragColor = vec4(resultColor, 1.0f);
	//计算高亮部分
	float brightness = dot(resultColor, vec3(0.2126f, 0.7152f, 0.0722f));
	if(brightness > 1.0)
		BrightColor = FragColor;
	else
		BrightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}
