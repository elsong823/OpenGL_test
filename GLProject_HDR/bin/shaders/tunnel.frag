#version 330 core

uniform sampler2D texDiffuse;

in VS_OUT
{
	vec2 texCoords;
	vec3 fragPos;
}fs_in;

out vec4 fs_outColor;

struct stLight
{
	vec3 lightPos;
	vec3 lightColor;
};

uniform stLight lights[4];

vec3 CalcLightColor(stLight _light);

void main()
{
	vec3 finalColor = vec3(0.0f);
	for(int i = 0; i < 4; ++i)
	{
		finalColor += CalcLightColor(lights[i]);
	}
	vec3 pixelColor = texture2D(texDiffuse, fs_in.texCoords).rgb;
	fs_outColor = vec4(pixelColor * (finalColor + vec3(0.15f)), 1.0f);
}

vec3 CalcLightColor(stLight _light)
{
	float constant = 1.0f;
	float linear = 0.7f;
	float quadratic = 1.8f;
	float fragToLight = length(fs_in.fragPos - _light.lightPos);
	float attenuation = 1.0f / (constant + fragToLight * linear + fragToLight * quadratic * quadratic);
	return _light.lightColor * attenuation;
}