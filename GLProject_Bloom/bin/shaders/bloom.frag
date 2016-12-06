#version 330 core

layout (location = 0) out vec4 FragColor;

uniform sampler2D texNormal;
uniform sampler2D texAffterBlur;
uniform float exposure;
uniform bool bloom;
in vec2 fragTexCoords;

void main()
{
	float gamma = 2.2f;
	vec3 hdrColor = texture2D(texNormal, fragTexCoords).rgb;
	vec3 bloomColor = texture2D(texAffterBlur, fragTexCoords).rgb;
	if(bloom)
	{
		hdrColor += bloomColor;
		//用曝光处理一下
		hdrColor = vec3(1.0f) - exp(-hdrColor * exposure);
		//hdrColor = pow(hdrColor, vec3(1.0 / gamma));
	}
	FragColor = vec4(hdrColor, 1.0f);
}
