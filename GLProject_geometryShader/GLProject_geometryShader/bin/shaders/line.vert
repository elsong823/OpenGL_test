
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;

out VS_OUT
{
	vec3 normal;
}vs_out;

void main()
{
	gl_Position = matProjection * matView * matModel * vec4(position, 1.0f);
	mat3 matNormal = mat3(transpose(inverse(matView * matModel)));
	vs_out.normal = vec3(matProjection * vec4(matNormal * normal, 1.0f));
}