#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;
varying vec3 cubeTexCoords;

void main()
{
	vec4 pos = matProjection * matView * vec4(position, 1.0f);
	gl_Position = pos.xyww;
	cubeTexCoords = position;
}