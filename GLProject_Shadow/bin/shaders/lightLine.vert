#version 330 core

layout (location = 0) in vec3 position;

uniform vec3 lightPos;

layout (std140) uniform Matrices
{
	mat4 matView;
	mat4 matProjection;
};
out VS_OUT
{
	vec4 lightPos;
}vs_out;

void main()
{
	vec3 dir = normalize(-lightPos);
	vec3 terminalPos = lightPos + dir * 10.0f;
	vs_out.lightPos = matProjection * matView * vec4(lightPos, 1.0f);
	gl_Position = matProjection * matView * vec4(terminalPos, 1.0f);
}
