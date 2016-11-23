#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;

varying vec3 fragCoord;

void main()
{
	gl_Position = matProjection * matView * vec4(position, 1.0f);
	gl_Position = gl_Position.xyww;
	fragCoord = position;
}