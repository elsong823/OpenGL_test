#version 330 core

struct Material
{
	sampler2D texDiffuse;
	sampler2D texSpecular;
	float shininess;
};

struct DirectLight
{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float constant;
	float linear;
	float quadratic;
};

struct SpotLight
{
	vec3 position;
	vec3 faceTo;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float cutoff;
	float outCutoff;
	float constant;
	float linear;
	float quadratic;
};

varying vec2 fragCoords;
varying vec3 fragPos;
varying vec3 fragNormal;
uniform vec3 viewPos;
uniform Material material;
uniform DirectLight directLight;
uniform PointLight pointLights[3];
uniform SpotLight spotLight;

out vec4 color;

vec3 CalcDirectLight(DirectLight _directLight, vec3 _normal, vec3 _viewPos);
vec3 CalcPointLight(PointLight _pointLight);
vec3 CalcSpotLight(SpotLight _spotLight);

void main()
{
	vec3 resultColor = CalcDirectLight(directLight, fragNormal, viewPos);
	for(int i = 0; i < 3; ++i)
	{
		resultColor += CalcPointLight(pointLights[i]);
	}
	resultColor += CalcSpotLight(spotLight);
	color = vec4(resultColor,1.0f);
	color = vec4(1.0f);
}

vec3 CalcDirectLight(DirectLight _directLight, vec3 _normal, vec3 _viewPos)
{
	vec3 ambientColor;
	vec3 diffuseColor;
	vec3 specularColor;
	vec3 normal = normalize(_normal);
	vec3 lightDir = normalize(_directLight.direction);
	vec3 reflectDir = reflect(lightDir, normal);
	vec3 viewDir = normalize(_viewPos - fragPos);
	vec3 texDiffuse = vec3(texture(material.texDiffuse, fragCoords));
	ambientColor = _directLight.ambient * texDiffuse;
	float diff = max(dot(-lightDir, normal),0.0f);
	diffuseColor = _directLight.diffuse * texDiffuse * diff;
	float spec = pow(max(dot(reflectDir, viewDir),0.0f), material.shininess);
	specularColor = _directLight.specular * vec3(texture(material.texSpecular, fragCoords)) * spec;
	return ambientColor + diffuseColor + specularColor;
}

vec3 CalcPointLight(PointLight _pointLight)
{
	vec3 lightDir = normalize(fragPos - _pointLight.position);
	vec3 normal = normalize(fragNormal);
	vec3 reflect = reflect(lightDir, normal);
	vec3 viewDir = normalize(viewPos - fragPos);

	vec3 ambientColor;
	vec3 diffuseColor;
	vec3 specularColor;
	float attenuation = 1.0f;
	float distance = length(fragPos - _pointLight.position);
	attenuation = 1.0f / (1.0f + distance * _pointLight.linear + distance * distance * _pointLight.quadratic);
	vec3 texDiffuse = vec3(texture(material.texDiffuse, fragCoords));


	ambientColor = _pointLight.ambient * texDiffuse;
	float diff = max(dot(-lightDir, normal),0.0f);
	diffuseColor = _pointLight.diffuse * texDiffuse * diff;
	float spec = pow(max(dot(reflect, viewDir),0.0f), material.shininess);
	specularColor = _pointLight.specular * vec3(texture(material.texSpecular, fragCoords)) * spec;

	return (ambientColor + diffuseColor + specularColor) * attenuation;
}

vec3 CalcSpotLight(SpotLight _spotLight)
{
	vec3 normal = normalize(fragNormal);
	vec3 lightDir = normalize(fragPos - _spotLight.position);
	vec3 reflectDir = reflect(lightDir, normal);
	vec3 spotDir = normalize(_spotLight.faceTo);
	vec3 texDiffuse = vec3(texture(material.texDiffuse, fragCoords));
	vec3 ambientColor = _spotLight.ambient * texDiffuse;

	float diff = max(dot(-lightDir, normal), 0.0f);
	vec3 diffuseColor = _spotLight.diffuse * texDiffuse * diff;
	float spec = pow(max(dot(-lightDir, reflectDir), 0.0f), material.shininess);
	vec3 specularColor = spotLight.specular * vec3(texture(material.texSpecular, fragCoords)) * spec;
	float distance = length(_spotLight.position -fragPos);
	float attenuation = 1.0f / (_spotLight.constant + distance * _spotLight.linear + distance * distance * _spotLight.quadratic);
	float theta = dot(lightDir, spotDir);
	theta = clamp((theta - _spotLight.outCutoff)/(_spotLight.cutoff - _spotLight.outCutoff),0.0f,1.0f);
	return (ambientColor + diffuseColor + specularColor) * attenuation * theta;
}