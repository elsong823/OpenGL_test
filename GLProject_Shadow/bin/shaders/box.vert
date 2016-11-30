#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

uniform mat4 matModel;

layout (std140) uniform Matrices
{
	mat4 matView;
	mat4 matProjection;
};
out VS_OUT
{
	vec3 fragPos;
	vec2 fragCoords;
	vec3 fragNormal;
	vec4 fragPosLightSpace;
}vs_out;

uniform mat4 matLightSpace;

void main()
{
	gl_Position = matProjection * matView * matModel * vec4(position, 1.0f);
	vs_out.fragCoords = texCoords;
	vs_out.fragPos = vec3(matModel * vec4(position, 1.0f));
	vs_out.fragNormal = transpose(inverse(mat3(matModel))) * normal;
	vs_out.fragPosLightSpace = matLightSpace * vec4(vs_out.fragPos, 1.0f);
}