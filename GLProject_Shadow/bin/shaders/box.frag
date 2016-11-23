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

uniform Material material;
uniform DirectLight dLight;

in vec2 fragCoords;
in vec3 fragPos;
in vec3 fragNormal;

out vec4 color;

vec3 CalcDirectionLight();

void main()
{
	color = texture2D(material.texDiffuse, fragCoords);
}

vec3 CalcDirectionLight()
{
	vec3 color;
	return color;
}



























































