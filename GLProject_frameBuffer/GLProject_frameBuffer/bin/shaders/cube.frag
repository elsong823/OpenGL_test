
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

varying vec2 fragTexCoord;
varying vec3 fragNormal;
varying vec3 fragPos;

uniform Material material;
uniform DirectLight directLight;
uniform vec3 eyePos;

out vec4 o_outColor;

vec3 CalcDirectLight(DirectLight _dLight);

void main()
{
	o_outColor = vec4(CalcDirectLight(directLight),1.0f);
}

vec3 CalcDirectLight(DirectLight _dLight)
{
	vec3 normal = normalize(fragNormal);
	vec3 lightDir = normalize(_dLight.direction);
	vec3 reflectLight = reflect(lightDir, normal);
	vec3 viewDir = normalize(eyePos - fragPos);
	vec3 texDiffuse = vec3(texture(material.texDiffuse, fragTexCoord));
	
	vec3 ambient = _dLight.ambient * texDiffuse;
	
	float diff = max( dot(-lightDir, normal),0.0f);
	vec3 diffuse = _dLight.diffuse * texDiffuse * diff;
	
	float spec = pow( max(dot(reflectLight, viewDir), 0.0f), material.shininess);
	vec3 specular = _dLight.specular * vec3(texture(material.texSpecular, fragTexCoord)) * spec;
	return ambient + diffuse + specular;
}