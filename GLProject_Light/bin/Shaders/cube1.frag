/*
#version 330 core

uniform vec3 objColor;
uniform vec3 lightColor;
uniform float ambientStrength;

in vec3 fragNormal;
in vec3 fragPos;
in vec3 fragLightPos;
uniform vec3 lightPos;
//uniform vec3 lightPos;
uniform vec3 viewPos;
uniform int shininess;
uniform float specularStength;
out vec4 outColor;

void main()
{
	//ambient
	vec3 _ambientLight = lightColor * ambientStrength;
	
	//diffuse
	vec3 _diffuseLight = max(dot( normalize(lightPos - fragPos), normalize(fragNormal) ),0.0f) * lightColor;

	//specular
	vec3 _reflectLight = reflect( normalize(fragPos - lightPos), fragNormal );
	float _specularPower = pow( max(dot(_reflectLight, normalize(viewPos - fragPos)),0.0f), shininess);
	vec3 _specularLight = lightColor * _specularPower * specularStength;

	vec3 _resultLight = _ambientLight + _diffuseLight + _specularLight;
	outColor = vec4(objColor * _resultLight, 1.0f);
}

*/

/*
#version 330 core

in vec3 fragColor;
uniform vec3 objColor;
out vec4 color;

void main()
{
	color = vec4(fragColor * objColor, 1.0f);
}
*/

//#version 330 core
//
//out vec4 color;
//
//uniform vec3 objColor;
//uniform vec3 lightColor;
//uniform vec3 eyePos;
//
//in vec3 fragNormal;
//in vec3 fragLightPos;
//in vec3 fragPos;
//
//void main()
//{
//	//ambient
//	float ambientStrength = 0.2f;
//	vec3 ambientLight = lightColor * ambientStrength;
//	
//	//diffuse
//	vec3 lightDir = normalize(fragPos - fragLightPos);
//	vec3 diffuseLight = lightColor * max(dot(-lightDir, fragNormal),0.0f);
//	
//	//specular
//	vec3 refelctDir = reflect(lightDir, fragNormal);
//	float spec = pow(dot(refelctDir, normalize(eyePos - fragPos)),128);
//	vec3 specularLight = lightColor * spec * 0.5f;
//	vec3 resultLightColor = ambientLight + diffuseLight + specularLight;
//	color = vec4(objColor * resultLightColor, 1.0f);
//}

/*
#version 330 core

uniform vec3 objColor;
in vec3 fragColor;
out vec4 color;

void main()
{
	color = vec4(objColor * fragColor, 1.0f);
}*/

#version 330 core

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

//Direct
struct DirctLight
{
	vec3 direct;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};	
//Point
struct PointLight
{
	vec3 position;
	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
//Spot
struct SpotLight
{
	vec3 position;
	vec3 lightDir;

	float constant;
	float linear;
	float quadratic;

	float cutoff;
	float outCutoff;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec3 fragPos;
in vec2 fragCoords;
in vec3 fragNormal;
uniform vec3 fragViewPos;
//material
uniform Material material;
//lights
uniform DirctLight directLight;
uniform PointLight pointLights[3];
uniform SpotLight spotLight;

out vec4 color;

vec3 CalcDirectLightColor(DirctLight _light, vec3 _normal, vec3 _viewDir);
vec3 CalcPointLightColor(PointLight _light, vec3 _fragPos, vec3 _normal, vec3 _viewDir);
vec3 CalcSpotLightColor(SpotLight _light, vec3 _fragPos, vec3 _normal, vec3 _viewDir);

void main()
{	
	vec3 viewDir = normalize(fragPos - fragViewPos);
	vec3 result = CalcDirectLightColor(directLight, fragNormal, viewDir);
	for(int i = 0; i < 3; ++i)
	{
		result += CalcPointLightColor(pointLights[i], fragPos, fragNormal, viewDir);
	}
	result += CalcSpotLightColor(spotLight, fragPos, fragNormal, viewDir);
	color = vec4(result, 1.0f);
}

vec3 CalcDirectLightColor(DirctLight _light, vec3 _normal, vec3 _viewDir)
{
	vec3 normal = normalize(_normal);
	vec3 viewDir = normalize(_viewDir);
	vec3 lightDir = normalize(_light.direct);
	vec3 lightReflect = reflect(lightDir, normal);

	//Calc ambient
	vec3 ambientColor = _light.ambient * vec3(texture(material.diffuse, fragCoords));

	//Calc diffuse
	float _diff = max(dot(-lightDir, normal), 0.0f);
	vec3 diffuseColor = _light.diffuse * _diff * vec3(texture(material.diffuse, fragCoords));

	//Calc specular
	float _spec = pow( max(dot(-viewDir, lightReflect), 0.0f),material.shininess);

	//Calc Result
	vec3 specularColor = _light.specular * _spec * vec3(texture(material.specular, fragCoords));
	vec3 resultColor = ambientColor + diffuseColor + specularColor;
	return resultColor;
}

vec3 CalcPointLightColor(PointLight _light, vec3 _fragPos, vec3 _normal, vec3 _viewDir)
{
	vec3 normal = normalize(_normal);
	vec3 lightDir = normalize(_fragPos - _light.position);
	vec3 viewDir = normalize(_viewDir);
	vec3 relect = reflect(lightDir, normal);
	float distance = length(_light.position - _fragPos);
	float attenuation = 1.0f / (_light.constant + _light.linear * distance + _light.quadratic * _light.quadratic * distance);
	vec3 texDiffuse = vec3(texture(material.diffuse, fragCoords));
	//
	vec3 ambient = _light.ambient * texDiffuse * attenuation * 0.1f;
	//
	float diff = max(dot(-lightDir, normal),0.0);
	vec3 diffuse = _light.diffuse * texDiffuse * diff * attenuation * 0.8f;
	float spec = pow(max(dot(relect, -viewDir),0.0f) ,material.shininess);
	vec3 specular = _light.specular * vec3(texture(material.specular, fragCoords)) * spec;

	return diffuse + specular;
}

vec3 CalcSpotLightColor(SpotLight _light, vec3 _fragPos, vec3 _normal, vec3 _viewDir)
{	
	vec3 normal = normalize(_normal);
	vec3 viewDir = normalize(_viewDir);
	vec3 lightDir = normalize(_fragPos - _light.position);
	vec3 reflect = reflect(lightDir, normal);
	vec3 texDiff = vec3(texture(material.diffuse, fragCoords));
	vec3 ambient = _light.ambient * texDiff;
	//
	float attenuation = 1.0f;
	float theta = dot(_light.lightDir, lightDir);
	attenuation = clamp((theta - _light.outCutoff) / (_light.cutoff - _light.outCutoff),0.0f, 1.0f);
	float intensity = 0.0f;
	float distance = length(_fragPos - _light.position);
	intensity = 1.0f / (_light.constant + _light.linear * distance + pow(_light.quadratic, 2) * distance);
	float _diff = max(dot(-lightDir, normal),0.0f);
	vec3 diffuse = _light.diffuse * _diff * texDiff * attenuation * intensity;
	float spec = pow(max(dot(reflect, -viewDir),0.0f), material.shininess);
	vec3 specular = _light.specular * vec3(texture(material.specular, fragCoords)) * spec * intensity;
	return ambient + diffuse + specular;		
}
