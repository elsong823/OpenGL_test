#version 330 core

layout (location = 0) out vec4 FragColor;

struct stLight
{
	vec3 lightPos;
	vec3 lightColor;
};

in vec2 fragTexCoord;
//都在世界空间中
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
//灯光
uniform stLight lights[32];
//观察者位置
uniform vec3 viewPos;

void main()
{
	vec3 fragPos = texture2D(gPosition, fragTexCoord).xyz;
	vec3 normal = normalize(texture2D(gNormal, fragTexCoord).xyz);
	vec3 albedo = texture2D(gAlbedoSpec, fragTexCoord).rgb;
	float specColor = texture2D(gAlbedoSpec, fragTexCoord).a;
	
	vec3 diffuse;
	vec3 specular;
	for(int i = 0; i < 32; ++i)
	{
		//diffuse
		vec3 lightDir = normalize(fragPos - lights[i].lightPos);
		//计算距离
		float d = distance(fragPos, lights[i].lightPos);
		//float d = length(fragPos - lights[i].lightPos);
		//计算衰减
		float attenuation = 1.0f / (d * d);
		float diff = max(dot(-lightDir, normal), 0.0f);
		diffuse += lights[i].lightColor * diff * albedo * attenuation * 0.5f;
		
		//specuar
		vec3 viewDir = normalize(fragPos - viewPos);
		vec3 halfway = normalize(-viewDir + -lightDir);
		float spec = pow(max(dot(halfway, normal), 0.0f), 64.0f);
		specular += lights[i].lightColor * spec * specColor * attenuation * 1.0f;
	}
	FragColor = vec4(diffuse + specular + albedo * 0.1f, 1.0f);
}

