#version 330 core

layout (location = 0) in vec3 position;

uniform vec3 dirLight;

layout (std140) uniform Matrices
{
	mat4 matView;
	mat4 matProjection;
};

out VS_OUT
{
	vec4 position;
}vs_out;

void main()
{
	vec3 dLight = -1 * normalize(dirLight);
	gl_Position = matProjection * matView * vec4(dLight * -15.0f, 1.0f);
	vs_out.position = matProjection * matView * vec4(dLight * 15.0f, 1.0f);
}