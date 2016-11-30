#version 330 core

uniform sampler2D texDiff;
uniform sampler2D texSpec;
uniform samplerCube texShadow;
uniform vec3 dirLight;
uniform vec3 viewPos;
uniform vec3 lightPos;
uniform float farPlane;
in VS_OUT
{
	vec2 texCoord;
	vec3 normal;	
	vec3 fragPos;
}fs_in;

out vec4 color;

float CalcShadow();

void main()
{
	float constant = 1.0f;
	float linear = 0.09f;
	float quadratic = 0.032f;
	vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
	vec3 normal = normalize(fs_in.normal);
	vec3 lightDir = normalize(fs_in.fragPos - lightPos);
	vec3 viewDir = normalize(fs_in.fragPos - viewPos);
	vec3 pixDiff = texture2D(texDiff, fs_in.texCoord).xyz;
	vec3 pixSpec = texture2D(texSpec, fs_in.texCoord).xyz;
	//ambient
	vec3 ambient = lightColor * pixDiff * 0.2f;
	
	float distance = length(fs_in.fragPos - lightPos);
	float attenuation = 1.0f / (constant + distance * linear + distance * distance * quadratic);
	//diffuse
	float diff = max(dot(-lightDir, normal), 0.0f);
	vec3 diffuse = lightColor * pixDiff * diff * 1.0f * attenuation;
	
	//specular
	vec3 halfway = normalize(-lightDir + -viewDir);
	float spec = pow(max(dot(halfway, normal), 0.0f), 64.0f);
	vec3 specular = lightColor * pixSpec * spec * 1.0f;
	
	//out color
	color = vec4((ambient + (diffuse + specular) * (1.0f - CalcShadow())), 1.0f);
}

float CalcShadow()
{
	//固定位置采样
	vec3 sampleOffsetDirections[20] = vec3[]
	(
	   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
	   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
	   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
	   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
	   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
	);
	float bias = 0.05f;
	float diskRadius = 0.05f;
	vec3 fragToLight = fs_in.fragPos - lightPos;
	float curDepth = length(fragToLight);
	float shadow = 0.0f;
	for(int i = 0; i < 20; ++i)
	{
		float depthInMap = texture(texShadow, fragToLight + diskRadius * sampleOffsetDirections[i]).r * farPlane;
		if(curDepth - bias > depthInMap)
		{
			shadow += 1.0f;
		}
	}
	return shadow / 20.0f;
}

