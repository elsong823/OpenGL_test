#version 330 core

layout (location = 0) in vec3 position;

layout (std140) uniform Matrices
{
	mat4 matView;
	mat4 matProjection;
	mat4 matLight;
};

uniform mat4 matModel;

void main()
{
	gl_Position = matProjection * matView * matModel * vec4(position, 1.0f);
}