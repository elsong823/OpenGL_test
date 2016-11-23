
#version 330 core

out vec4 outColor;

uniform vec3 eyePos;
uniform samplerCube skyBox;

varying vec3 fragNormal;
varying vec3 fragPos;

void main()
{
	float ratio = 1.0f / 1.52f;
	vec3 refractDir = refract(normalize(fragPos - eyePos), normalize(fragNormal), ratio);
	outColor = texture(skyBox, refractDir);
}