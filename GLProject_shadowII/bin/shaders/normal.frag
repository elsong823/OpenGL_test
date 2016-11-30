#version 330 core

uniform sampler2D texDiff;
uniform vec3 dirLight;
uniform sampler2D texShadow;

in VS_OUT
{
	vec2 texCoord;
	vec3 normal;	
	vec3 fragPos;
	vec4 fragLightSpace;
}fs_in;

out vec4 color;

float CalcShadow();

void main()
{
	vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
	vec3 pixelColor = texture2D(texDiff, fs_in.texCoord).rgb;
	vec3 dLight = normalize(dirLight);
	vec3 normal = normalize(fs_in.normal);
	//Ambient
	vec3 ambient = lightColor * 0.15f;
	//diffuse
	float diff = max( dot(-dLight, normal),0.0f);
	vec3 diffuse = lightColor * diff * 1.0f;
	float shadow = CalcShadow();
	color = vec4((ambient + (1.0f - shadow) * diffuse) * pixelColor, 1.0f);
}

float CalcShadow()
{
	vec3 coord = fs_in.fragLightSpace.xyz / fs_in.fragLightSpace.w;
	vec2 offset = 1.0f / textureSize(texShadow, 0);
	coord = coord * 0.5f + 0.5f;
	float curDepth = coord.z;
	float shadow = 0.0f;
	if(curDepth > 1.0f)
	{
		shadow = 0.0f;
	}
	else
	{	
		for(int x = -1; x <= 1; ++x)
		{
			for(int y = -1; y <= 1; ++y)
			{	
				float depthInMap = texture2D(texShadow, coord.xy + vec2(x, y)*offset).r;
				shadow += curDepth > depthInMap ? 1.0f : 0.0f;
			}
		}
	}
	return shadow / 9.0f;
}