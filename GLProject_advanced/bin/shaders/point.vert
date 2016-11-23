#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;

varying VS_OUT
{
	vec3 color;
}vs_out;


void main()
{
	gl_Position = matProjection * matView * matModel * vec4(position, 1.0f);
	vs_out.color = vec3(0.0f,1.0f,0.0f);
}