#version 330 core

layout (location = 0) out vec4 FragColor;

in vec2 fragTexCoords;

uniform sampler2D gPositionDepth;	//位置、深度
uniform sampler2D gNormal;	//法线
uniform sampler2D gAlbedo;	//漫反射率
uniform sampler2D ssao;		//遮蔽

uniform bool useSsao;

struct stLight
{
	//注意这个位置是在 观察者空间 中
	vec3 position;
	vec3 color;
	float constant;
	float linear;
	float quadratic;
};

uniform stLight light;

void main()
{
	//从g缓冲中提取数值
	//注意：这是在 观察者空间 中
	vec3 fragPos = texture(gPositionDepth, fragTexCoords).xyz;
	vec3 lightDir = normalize(fragPos - light.position);
	//由于是在观察者空间，因此fragPos就是视线的方向
	vec3 viewDir = normalize(fragPos);
	vec3 normal = normalize( texture(gNormal, fragTexCoords).xyz );
	vec3 halfway = normalize(-lightDir + -viewDir);
	
	//光计算
	vec3 pixelColor = texture(gAlbedo, fragTexCoords).rgb;
	//环境光
	vec3 ambient= light.color * pixelColor * 0.3;
	if(useSsao)
	{
		ambient *= texture(ssao, fragTexCoords).r;
	}
	
	//漫反射
	float diff = max(dot(-viewDir, normal), 0.0f);
	vec3 diffuse = light.color * pixelColor * 1.0 * diff;
	
	//镜面反射
	float spec = pow(max(dot(halfway, normal), 0.0), 8.0);
	vec3 specular = light.color * 1.0 * spec;
	
	//衰减
	float dis = distance(fragPos, light.position);
	float attenuation = 1.0 / (1.0 + dis * light.linear + dis * dis * light.quadratic);
	diffuse *= attenuation;
	specular *= attenuation;
	FragColor = vec4(ambient + diffuse + specular, 1.0);
}