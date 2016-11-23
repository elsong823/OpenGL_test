
#version 330 core

layout (location = 0) in vec3 position;

layout (std140) uniform Matries
{
	vec3 color;
	mat4 matView;
	mat4 matProjection;
	mat4 matModel;
};

varying vec3 fragColor;
	
void main()
{
	gl_Position = matProjection * matView * matModel * vec4(position, 1.0f);
	fragColor = color;
}