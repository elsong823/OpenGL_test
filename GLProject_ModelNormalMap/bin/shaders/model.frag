
#version 330 core

uniform sampler2D tex_diffuse1;
uniform sampler2D tex_specular1;
uniform sampler2D tex_normalMap1;
uniform sampler2D tex_reflect1;

out vec4 color;

in VS_OUT
{
	vec2 texcoords;
	vec3 fragPos;
	vec3 lightDir;
	vec3 viewPos;
	mat3 matTangent;
	vec3 normal2;
}fs_in;

void main()
{
	vec3 light = vec3(1.0f);
	vec3 specularLight = vec3(0.0f, 1.0f, 1.0f);
	vec3 resultColor;
	vec3 pixelDiffuseColor = texture2D(tex_diffuse1, fs_in.texcoords).rgb;
	vec3 pixelSpecularColor = texture2D(tex_specular1, fs_in.texcoords).rgb;
	vec3 pixelNormalColor = texture2D(tex_normalMap1, fs_in.texcoords).rgb;
	vec3 normal = pixelNormalColor;
	normal = normalize(normal * 2.0f - 1.0f);
	normal = normalize(fs_in.normal2);
	//normal = fs_in.matTangent * normal;
	//ambient
	vec3 ambientColor = light * pixelDiffuseColor * 0.15f;
	
	
	//计算光线
	vec3 lightDir = normalize(fs_in.lightDir);
	vec3 viewDir = normalize(fs_in.fragPos - fs_in.viewPos);
	float diff = max(dot(normal, -lightDir), 0.0f);
	//diffuse
	vec3 diffuseColor = light * pixelDiffuseColor * diff * 0.5f;

	//specular
	vec3 halfway = normalize(-lightDir + -viewDir);
	float spec = pow(max(dot(halfway, normal), 0.0f), 128.0f);
	vec3 specularColor = specularLight * pixelSpecularColor * spec * 1.0f;
	color = vec4(specularColor, 1.0f);
}
