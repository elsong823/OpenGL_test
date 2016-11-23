#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;

varying vec3 fragNormal;
varying vec3 fragPos;

void main()
{
	gl_Position = matProjection * matView * matModel * vec4(position,1.0f);
	fragNormal = mat3(transpose(inverse(matModel))) * normal;
	fragPos = vec3(matModel * vec4(position,1.0f));
}