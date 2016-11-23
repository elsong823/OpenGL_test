
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;

varying vec2 fragTexCoord;
varying vec3 fragNormal;
varying vec3 fragPos;

void main()
{
	gl_Position = vec4(matProjection * matView * matModel * vec4(position, 1.0f));
	fragNormal = mat3(transpose(inverse(matModel))) * normal;
	fragPos = vec3(matModel * vec4(position, 1.0f));
	fragTexCoord = texCoords;
}