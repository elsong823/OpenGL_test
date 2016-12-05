
#version 330 core

struct PointLight
{
	vec3 lightPosition;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float constant;
	float linear;
	float quadratic;
};

struct DirectLight
{
	vec3 lightDir;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct SpotLight
{
	vec3 position;
	vec3 faceTo;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float constant;
	float linear;
	float quadratic;
	float cutoff;
	float outCutoff;
};

uniform float shiniess;
uniform vec3 viewPos;
uniform DirectLight dLight;
uniform PointLight ptLights[2];
uniform SpotLight spotLight;
uniform sampler2D tex_diffuse1;
uniform sampler2D tex_specular1;

varying vec2 fragCoords;
varying vec3 fragPos;
varying vec3 fragNormal;
out vec4 color;

vec3 CalcDLight(DirectLight _dLight);
vec3 CalcPtLight(PointLight _ptLight);
vec3 CalcSLight(SpotLight _sLight);

void main()
{
	vec3 resultColor = CalcDLight(dLight);
	for(int i = 0; i < 2; ++i)
	{
		resultColor += CalcPtLight(ptLights[i]);
	}
	resultColor += CalcSLight(spotLight);
	color = vec4(resultColor, 1.0f);
}

vec3 CalcDLight(DirectLight _dLight)
{
	vec3 normal = normalize(fragNormal);
	vec3 lightDir = normalize(_dLight.lightDir);
	vec3 reflectDir = reflect(lightDir, normal);
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 texDif = vec3(texture(tex_diffuse1, fragCoords));
	vec3 ambientColor = texDif * _dLight.ambient;

	float diff = max( dot(-lightDir, normal),0.0f);
	vec3 diffuseColor = texDif * _dLight.diffuse * diff;

	float spec = pow( max( dot(reflectDir, viewDir),0.0f), shiniess);
	vec3 specularColor = vec3(texture(tex_specular1, fragCoords)) * _dLight.specular * spec;

	return ambientColor + diffuseColor + specularColor;
}

vec3 CalcPtLight(PointLight _ptLight)
{
	vec3 normal = normalize(fragNormal);
	vec3 lightDir = normalize(fragPos - _ptLight.lightPosition);
	vec3 reflectDir = reflect(lightDir, normal);
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 texDif = vec3(texture(tex_diffuse1, fragCoords));
	vec3 ambientColor = texDif * _ptLight.ambient;

	float diff = max( dot(-lightDir, normal),0.0f);
	vec3 diffuseColor = texDif * _ptLight.diffuse * diff;

	float spec = pow( max( dot(reflectDir, viewDir),0.0f), shiniess);
	vec3 specularColor = vec3(texture(tex_specular1, fragCoords)) * _ptLight.specular * spec;

	float distance = length(fragPos - _ptLight.lightPosition);
	float attenuation = 1.0f / (_ptLight.constant + distance * _ptLight.linear + distance * distance * _ptLight.quadratic);
	return (ambientColor + diffuseColor + specularColor)*attenuation;
}

vec3 CalcSLight(SpotLight _sLight)
{
	vec3 normal = normalize(fragNormal);
	vec3 lightDir = normalize(fragPos - _sLight.position);
	vec3 reflect = reflect(lightDir, normal);
	vec3 lightFaceTo = normalize(_sLight.faceTo);
	vec3 viewDir = lightDir;
	vec3 texDiff = vec3(texture(tex_diffuse1, fragCoords));
	vec3 texSpec = vec3(texture(tex_specular1, fragCoords));
	float distance = length(fragPos - _sLight.position);
	float diff = max(dot(-lightDir, normal), 0.0f);
	float attenuation = 1.0f / (_sLight.constant + _sLight.linear * distance + _sLight.quadratic * distance * distance);
	float theta = max(dot(lightFaceTo, lightDir),0.0f);
	float intensity = clamp((theta - _sLight.outCutoff) / (_sLight.cutoff - _sLight.outCutoff), 0.0f, 1.0f);
	float spec = pow(max( dot(reflect, -viewDir),0.0f), shiniess);
	vec3 ambient = texDiff * _sLight.ambient;
	vec3 diffuse = texDiff * _sLight.diffuse * diff * attenuation * intensity;
	vec3 specular = texSpec * _sLight.specular *spec *  attenuation * intensity;
	return ambient + diffuse + specular;
	//if(theta > _sLight.cutoff)
		//return vec3(1.0f);
	//return vec3(0.0f);

}