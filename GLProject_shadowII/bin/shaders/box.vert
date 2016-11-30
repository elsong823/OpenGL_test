#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in mat4 matModel;

layout (std140) uniform Matrices
{
	mat4 matView;
	mat4 matProjection;
	mat4 matLight;
};

out VS_OUT
{
	vec2 texCoord;
	vec3 normal;
	vec3 fragPos;
	vec4 fragLightSpace;
}vs_out;

void main()
{
	gl_Position = matProjection * matView * matModel * vec4(position, 1.0f);
	vs_out.texCoord = texCoord;
	vs_out.fragPos = (matModel * vec4(position, 1.0f)).xyz;
	vs_out.normal = transpose(inverse(mat3(matModel))) * normal;
	vs_out.fragLightSpace = matLight * vec4(vs_out.fragPos, 1.0f);
}