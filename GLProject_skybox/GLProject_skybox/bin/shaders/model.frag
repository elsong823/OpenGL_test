
#version 330 core

uniform sampler2D texDiffuse_1;
uniform sampler2D texSpecular_1;
uniform sampler2D texReflect_1;
uniform int renderType;
uniform vec3 eyePos;
uniform samplerCube texSkyCube;

varying vec2 fragTexCoords;
varying vec3 fragNormal;
varying vec3 fragPos;

out vec4 outColor;

void main()
{
	vec4 rColor;
	if(renderType == 1)
	{
		vec3 reflectDir = reflect(normalize(fragPos - eyePos),normalize(fragNormal));
		rColor = texture(texSkyCube, reflectDir);
	}
	else if(renderType == 2)
	{
		float ratio = 1.0f / 1.52f;
		vec3 refractDir = refract(normalize(fragPos - eyePos),normalize(fragNormal), ratio);
		rColor = texture(texSkyCube, refractDir);
	}
	vec3 reflectInstance = vec3(texture(texReflect_1, fragTexCoords));
	rColor *= reflectInstance;
	vec3 diffuseColor = vec3(texture(texDiffuse_1, fragTexCoords));
	outColor = vec4(diffuseColor + rColor.rgb, 1.0f);
}