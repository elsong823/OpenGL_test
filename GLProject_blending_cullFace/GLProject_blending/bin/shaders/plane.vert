
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;

varying vec3 v_fragColor;

void main()
{
	gl_Position = vec4(matProjection * matView * matModel * vec4(position, 1.0f));
	v_fragColor = vec3(0.5f, 0.5f, 0.5f);
}