#version 330 core

out vec4 color;
in vec4 fragPos;
uniform vec3 lightPos;
uniform float farPlane;

void main()
{
	float distance = length(lightPos - fragPos.xyz);
	distance /= farPlane;
	gl_FragDepth = distance;
}