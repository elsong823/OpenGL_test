/*
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;
uniform mat4 matNormal;

uniform vec3 lightPos;

out vec3 fragNormal;
out vec3 fragPos;
out vec3 fragLightPos;

void main()
{
	gl_Position = matProjection * matView * matModel * vec4(position, 1.0f);
	fragNormal = mat3(matNormal) * normal ;
	fragPos = vec3(matModel * vec4(position, 1.0f));
	//fragLightPos = vec3(matView * vec4(lightPos, 1.0f));
	fragLightPos = lightPos;
}

*/

/*
//Gouraud
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;
uniform mat4 matNormal;

//color
uniform vec3 lightColor;
uniform vec3 vecLightPos;
uniform vec3 vecEyePos;
uniform float ambientStrength;
uniform float specularStrength;
uniform int shininess;

out vec3 fragColor;

void main()
{
	//Pos
	gl_Position = matProjection * matView * matModel * vec4(position, 1.0f);


	//Color
	vec3 _Normal = normalize(mat3(matNormal) * normal);
	//Ambient
	vec3 _ambientColor = lightColor * ambientStrength;
	//Diffuse
	vec3 _vertexWorldPos = vec3(matModel * vec4(position, 1.0f));
	vec3 _diffuseColor = lightColor * max(dot(normalize(vecLightPos - _vertexWorldPos), _Normal),0.0f);
	//Specular
	vec3 _reflectLight = reflect(normalize(_vertexWorldPos - vecLightPos), _Normal);
	float _spec = pow(max(dot(_reflectLight, normalize(vecEyePos - _vertexWorldPos)),0.0f),shininess);
	vec3 _specularColor = lightColor * _spec * 1.0f;
	fragColor = _ambientColor + _diffuseColor + _specularColor;
}

*/

//#version 330 core
//
//layout (location = 0) in vec3 position;
//layout (location = 1) in vec3 normal;
//
//uniform mat4 matModel;
//uniform mat4 matView;
//uniform mat4 matProjection;
//uniform mat4 matNormal;
//uniform vec3 lightPos;
//
//out vec3 fragNormal;
//out vec3 fragLightPos;
//out vec3 fragPos;
//out vec3 fragEyePos;
//
//void main()
//{
//	gl_Position = matProjection * matView * matModel * vec4(position, 1.0f);
//	fragNormal = normalize(mat3(transpose(inverse(matModel))) * normal);
//	fragLightPos = lightPos;
//	fragPos = vec3(matModel * vec4(position, 1.0f));
//}
/*
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;
uniform mat4 matNormal;
uniform vec3 lightPos;
uniform vec3 eyePos;
uniform vec3 lightColor;

out vec3 fragColor;

void main()
{
	gl_Position = matProjection * matView * matModel * vec4(position, 1.0f);
	
	vec3 ambientColor = lightColor * 0.2f;

	vec3 Normal = normalize(transpose(inverse(mat3(matModel))) * normal);
	vec3 lightDir = normalize(vec3(matModel * vec4(position, 1.0f)) - lightPos);
	vec3 diffuseColor = lightColor * max(dot(-lightDir, Normal),0.0f);

	vec3 _reflect = reflect(lightDir, Normal);
	float _spec = pow(max(dot(_reflect, normalize(eyePos - vec3(matModel * vec4(position, 1.0f)))),0.0f),128);
	vec3 specularColor = lightColor * _spec * 0.5f;
	fragColor = ambientColor + diffuseColor + specularColor;
}
*/

#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;

out vec3 fragNormal;
out vec3 fragPos;
out vec2 fragCoords;


void main()
{
	gl_Position = matProjection * matView * matModel * vec4(position, 1.0f);
	fragNormal = normalize(transpose(inverse(mat3(matModel))) * normal);
	fragPos = vec3(matModel * vec4(position, 1.0f));
	fragCoords = texCoords;
}
