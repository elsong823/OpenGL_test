#version 330 core

struct SpotLight
{
	vec3 position;
	vec3 dir;
	float cutoff;
	float outCutoff;
	float constant;
	float linear;
	float quadratic;
};

out vec4 color;
uniform sampler2D texDiffuse;
uniform sampler2D texNormal;
uniform SpotLight spotLight;
uniform vec3 viewPos;
uniform int type;
in vec3 fragPos;
in vec3 fragNormal;

in VS_OUT
{
	vec2 texCoord;
	mat3 matTBN;
	vec3 tangentLightPos;
	vec3 tangentViewPos;
	vec3 tangentFragPos;
	vec3 tangentSpotDir;
}fs_in;

void main()
{
	vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
	vec3 specLightColor = vec3(1.0f, 1.0f, 0.0f);
	vec3 pixColor = texture2D(texDiffuse, fs_in.texCoord).rgb;
	vec3 normal;
	if(type == 1)
	{
		//使用顶点法线
		normal = normalize(fragNormal);
	}
	else
	{
		//使用法线贴图
		normal =  texture2D(texNormal, fs_in.texCoord).xyz;
		normal = normalize(normal * 2.0f - 1.0f);
		//方法一：用传入的矩阵直接变化法线！
		//normal = fs_in.matTBN * normal;
		//方法二：传入了被转换到切线空间的点，因此不用再处理法线！
	}
	
	//方法一：
	//vec3 lightDir = normalize(fragPos - spotLight.position);
	//vec3 viewDir = normalize(fragPos - viewPos);
	//方法二：
	vec3 lightDir = normalize(fs_in.tangentFragPos - fs_in.tangentLightPos);
	vec3 viewDir = normalize(fs_in.tangentFragPos - fs_in.tangentViewPos);
	
	//ambient 
	vec3 ambient = lightColor * pixColor * 0.15f;
	
	//attenuation
	float fragToLight = length(spotLight.position - fragPos);
	float attenuation = 1.0f / (spotLight.constant + spotLight.linear * fragToLight + spotLight.quadratic * fragToLight * fragToLight);
	float theta = max(dot(lightDir, fs_in.tangentSpotDir), 0.0f);
	float spotAttenuation = clamp((theta - spotLight.outCutoff)/(spotLight.cutoff - spotLight.outCutoff), 0.0f, 1.0f);
	
	//diffuse
	float diff =  dot(-lightDir, normal);
	vec3 diffuse = lightColor * pixColor * diff * attenuation * spotAttenuation * 0.85f;
	
	//specular
	vec3 halfway = normalize(-lightDir + -viewDir);
	float spec = pow( max(dot(halfway, normal), 0.0f), 128.0f);
	vec3 specular = specLightColor * pixColor * spec * attenuation * spotAttenuation * 1.0f;
	
	color = vec4(ambient + diffuse + specular, 1.0f);
}