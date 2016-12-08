#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;

out VS_OUT
{
	vec2 texCoords;
	vec3 fragPos;
	vec3 normal;
}vs_out;

void main()
{
	gl_Position = matProjection * matView * matModel * vec4(position, 1.0f);
	vs_out.texCoords = texCoords;
	vs_out.fragPos = (matView * matModel * vec4(position, 1.0f)).xyz;
	vs_out.normal = transpose(inverse(mat3(matView * matModel))) * normal;
}