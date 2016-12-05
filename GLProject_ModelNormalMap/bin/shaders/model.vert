
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texcoords;
layout (location = 2) in vec3 tangent;
layout (location = 3) in vec3 bitangent;
layout (location = 4) in vec3 normal;
layout (location = 5) in vec3 normal2;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;

uniform vec3 lightDir;
uniform vec3 viewPos;

out VS_OUT
{
	vec2 texcoords;
	vec3 fragPos;
	vec3 lightDir;
	vec3 viewPos;
	mat3 matTangent;
	vec3 normal2;
}vs_out;

void main()
{
	gl_Position = matProjection * matView * matModel * vec4(position , 1.0f);
	
	mat3 matNormal = transpose(inverse(mat3(matModel)));
	vec3 T = normalize(matNormal * tangent);
	vec3 B = normalize(matNormal * bitangent);
	vec3 N = normalize(matNormal * normal);
	mat3 matTangent = mat3(T, B, N);
	vs_out.texcoords = texcoords;
	vs_out.fragPos = vec3(matModel * vec4(position, 1.0f));
	vs_out.lightDir = lightDir;
	vs_out.viewPos = viewPos;
	vs_out.matTangent = matTangent;
	vs_out.normal2 = matNormal * normal2;
}
