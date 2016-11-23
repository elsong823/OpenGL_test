#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 matModel;
layout (std140) uniform Matrices
{
	mat4 matView;
	mat4 matProjection;
};

void main()
{
	gl_Position = matProjection * matView * matModel * vec4(position, 1.0f);
}