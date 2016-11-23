
#version 330 core

out vec4 outColor;

uniform vec3 eyePos;
uniform samplerCube skyBox;

varying vec3 fragNormal;
varying vec3 fragPos;

void main()
{
	//outColor = vec4(1.0f,1.0f,0.0f,1.0f);
	vec3 reflectDir = reflect(normalize(fragPos - eyePos), normalize(fragNormal));
	outColor = texture(skyBox, reflectDir);
}