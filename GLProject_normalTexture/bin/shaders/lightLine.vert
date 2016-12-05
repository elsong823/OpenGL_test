#version 330 core

layout (location = 0) in vec3 position;

uniform vec3 lightPos;
uniform vec3 lightDir;

layout (std140) uniform Matrices
{
	mat4 matView;
	mat4 matProjection;
};

out VS_OUT
{
	vec4 lightStart;
	vec4 lightEnd;
}vs_out;  

void main()
{
	gl_Position = vec4(0.0f);
	vs_out.lightStart = matProjection * matView * vec4(lightPos, 1.0f);
	vs_out.lightEnd = matProjection * matView * vec4(normalize(lightDir) * 5.0f + lightPos, 1.0f);
}