#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

uniform mat4 matModel;

layout (std140) uniform abc
{
	mat4 matView2;
	mat4 matProjection2;
};
out vec2 fragTexCoord;

void main()
{
	gl_Position = matProjection2 * matView2 * matModel * vec4(position, 1.0f);
	fragTexCoord = texCoord;
}