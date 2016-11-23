#version 330 core

uniform sampler2D texDiff;
uniform sampler2D texShadow;
uniform mat4 matLightSpace;
uniform vec3 lightPos;
uniform vec3 viewPos;
out vec4 color;

in VS_OUT
{
	vec3 fragPos;
	vec2 fragCoords;
	vec3 fragNormal;
	vec4 fragPosLightSpace;
}vs_in;

float CalculateShadow();

void main()
{
	vec3 light = vec3(1.0f);
	vec3 pixelColor = texture2D(texDiff, vs_in.fragCoords).rgb;
	color = vec4(0.0f);
	vec3 lightDir = normalize(vs_in.fragPos - lightPos);
	vec3 viewDir = normalize(vs_in.fragPos - viewPos);
	vec3 normal = normalize(vs_in.fragNormal);
	//ambient
	color += vec4(pixelColor * 0.15f, 1.0f);
	//diffuse
	float diff = max( dot(-lightDir, normal),0.0f);
	color += vec4(pixelColor * diff, 1.0f);
	//specular
	vec3 halfwayDir = normalize(-lightDir + -viewDir);
	float spec = pow( max( dot(halfwayDir, normal),0.0f), 32.0f);
	color +=  vec4(pixelColor * spec * 1.0f, 1.0f);
}

float CalculateShadow()
{

	return 0.0f;
}