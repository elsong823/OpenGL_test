#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

layout (std140) uniform Matrices
{
	mat4 matView;
	mat4 matProjection;
};

out VS_OUT
{
	vec2 texCoord;
	vec3 fragPos;
}vs_out;

uniform mat4 matModel;

void main()
{
	gl_Position = matProjection * matView * matModel * vec4(position, 1.0f);
	vs_out.texCoord = texCoord;
	vs_out.fragPos = vec3(matModel * vec4(position, 1.0f));
}