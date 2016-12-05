#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 matView;
uniform mat4 matProjection;

uniform vec3 lightPos;
uniform vec3 lightDir;

out VS_OUT
{
	vec4 lightStart;
	vec4 lightEnd;
}vs_out;

void main()
{
	vs_out.lightStart = matProjection * matView * vec4(lightPos, 1.0f);
	vs_out.lightEnd = matProjection * matView * vec4((lightPos + normalize(lightDir) * 10.0f),1.0f);
}