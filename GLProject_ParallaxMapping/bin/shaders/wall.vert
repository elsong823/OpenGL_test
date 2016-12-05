
#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texcoords;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;

uniform vec3 eyePos;
uniform vec3 lightDir;
uniform vec3 tangent;
uniform vec3 bitangent;

out VS_OUT
{
	vec2 texCoord;
	vec3 eyePos;
	vec3 lightDir;
	vec3 fragPos;
}vs_out;

void main()
{
	gl_Position = matProjection * matView * matModel * vec4(position, 0.0f, 1.0f);
	mat3 matNormal = transpose(inverse(mat3(matModel)));
	vec3 T = normalize(matNormal * tangent);
	vec3 B = normalize(matNormal * bitangent);
	vec3 N = normalize(cross(T, B));
	mat3 matTangent = mat3(T, B, N);
	matTangent = transpose(matTangent);
	vs_out.texCoord = texcoords;
	vs_out.eyePos = matTangent * eyePos;
	vs_out.lightDir = matTangent * lightDir;
	vs_out.fragPos = matTangent * (matModel * vec4(position, 0.0f,  1.0f)).xyz;
}
