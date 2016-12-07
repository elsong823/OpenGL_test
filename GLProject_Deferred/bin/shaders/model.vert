
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoords;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;

out VS_OUT
{
	vec3 fragPos;
	vec3 fragNormal;
	vec2 fragCoords;
}vs_out;

void main()
{
	gl_Position = matProjection * matView * matModel * vec4(position , 1.0f);
	vs_out.fragCoords = texcoords;
	vs_out.fragPos = vec3(matModel * vec4(position , 1.0f));
	vs_out.fragNormal = mat3(transpose(inverse(matModel))) * normal;
}
