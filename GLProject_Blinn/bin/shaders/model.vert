
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoords;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;
varying vec2 fragCoords;
varying vec3 fragPos;
varying vec3 fragNormal;


void main()
{
	gl_Position = matProjection * matView * matModel * vec4(position , 1.0f);
	fragCoords = texcoords;
	fragPos = vec3(matModel * vec4(position , 1.0f));
	fragNormal = mat3(transpose(inverse(matModel))) * normal;
}
