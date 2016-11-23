#version 330 core

in vec2 v_fragTexCoord;

uniform sampler2D u_fragTexture;
uniform vec3 lightPos;
uniform vec3 eyePosition;
uniform int blinn;
in vec3 fragPos;
in vec3 fragNormal;

out vec4 o_outColor;

void main()
{
	vec3 pixelColor = texture2D(u_fragTexture, v_fragTexCoord).rgb;
	vec3 lightDir = normalize(fragPos - lightPos);
	vec3 normal = normalize(fragNormal);
	vec3 viewDir = normalize(fragPos - eyePosition);
	//ambient
	o_outColor = vec4(pixelColor * 0.05f, 1.0f);
	//diffuse
	float diff = max(dot(-lightDir, normal),0.0f);
	o_outColor += vec4(pixelColor * diff * 0.5, 1.0f);
	//spec
	vec3 reflectDir = reflect(lightDir, normal);
	vec3 halfReflect = normalize(-lightDir + -viewDir);
	float spec;
	float specIntensity;
	if(blinn == 1)
	{
		spec = max(dot(halfReflect, normal), 0.0f);
		specIntensity = pow(spec, 16.0f);
	}
	else
	{
		spec = max(dot(reflectDir, -viewDir), 0.0f);
		specIntensity = pow(spec, 8.0f);
	}
	
	o_outColor += vec4(pixelColor * specIntensity * 0.8f, 1.0f);
}