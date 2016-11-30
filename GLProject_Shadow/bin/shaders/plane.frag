#version 330 core

uniform sampler2D texDiff;
uniform sampler2D texShadow;
uniform vec3 fragLightDir;
uniform vec3 viewPos;
out vec4 color;

in VS_OUT
{
	vec3 fragPos;
	vec2 fragCoords;
	vec3 fragNormal;
	vec4 fragPosLightSpace;
}vs_in;

float CalculateShadow();

void main()
{
	vec3 light = vec3(1.0f);
	vec3 pixelColor = texture2D(texDiff, vs_in.fragCoords).rgb;
	vec3 lightDir = normalize( fragLightDir );
	vec3 viewDir = normalize(vs_in.fragPos - viewPos);
	vec3 normal = normalize(vs_in.fragNormal);
	//ambient
	vec3 ambient = pixelColor * 0.15f;
	//diffuse
	float diff = max( dot(-lightDir, normal),0.0f);
	vec3 diffuse = light * diff * pixelColor;
	//specular
	vec3 halfwayDir = normalize(-lightDir + -viewDir);
	float spec = pow( max( dot(halfwayDir, normal),0.0f), 64.0f);
	vec3 specular = light * spec * pixelColor;
	float shadow = CalculateShadow();
	color = vec4(ambient + (1.0f - shadow)*(diffuse + specular) ,1.0f);
}

float CalculateShadow()
{
	vec3 newFragCoord = vs_in.fragPosLightSpace.xyz / vs_in.fragPosLightSpace.w;
	newFragCoord = newFragCoord * 0.5f + 0.5f;
	float depthInMap = 0.0f;
	vec2 texelSize = 1.0f / textureSize(texShadow, 0);
	float curDepth = newFragCoord.z;
	float shadow = 0.0f;
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			depthInMap = texture2D(texShadow, newFragCoord.xy + vec2(x, y) * texelSize).r;
			shadow += curDepth > (depthInMap + 0.0005) ? 0.5 : 0.0;
		}
	}
	if(curDepth > 1.0f)
	{
		shadow = 0;
	}
	return shadow / 9.0f;
}