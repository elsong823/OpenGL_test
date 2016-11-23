
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoords;
layout (location = 3) in mat4 instanceMatrix;

layout (std140) uniform Matrices
{
	mat4 matView;
	mat4 matProjection;
};

out vec2 fragTexCoord;

void main()
{
	gl_Position = matProjection * matView * instanceMatrix * vec4(position , 1.0f);
	fragTexCoord = texcoords;
}
