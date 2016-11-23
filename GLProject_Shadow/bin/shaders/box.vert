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

out vec2 fragCoords;
out vec3 fragPos;
out vec3 fragNormal;

void main()
{
	gl_Position = matProjection * matView * matModel * vec4(position, 1.0f);
	fragCoords = texCoords;
}