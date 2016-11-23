#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;

out vec2 outCoord;
out vec2 fragCoord;

void main()
{
	gl_Position = matProjection * matView * vec4(position, 1.0f);
	//outCoord = texCoords;
	fragCoord = texCoords;
}