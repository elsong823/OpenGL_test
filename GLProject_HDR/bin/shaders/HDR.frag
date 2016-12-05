#version 330 core

uniform sampler2D texHDR;

in VS_OUT
{
	vec2 texCoords;
}fs_in;

out vec4 fs_outColor;
uniform int HDR_type = 4;
//曝光度
uniform float exposure = 1.0f;
void main()
{
	if(HDR_type == 4)
	{
		//不用任何HDR技术
		fs_outColor = texture2D(texHDR, fs_in.texCoords);
	}
	//Reinhard算法
	else if(HDR_type == 5)
	{
		const float gamma = 2.2f;
		vec3 hdrColor = texture2D(texHDR, fs_in.texCoords).rgb;
		//Reinhard算法
		vec3 mapped = hdrColor / (hdrColor + vec3(1.0f));
		
		//Gamma还原
		//mapped = pow(mapped, vec3(1.0f/gamma));
		fs_outColor = vec4(mapped, 1.0f);	
	}
	//曝光度算法
	else if(HDR_type == 6)
	{
		vec3 hdrColor = texture2D(texHDR, fs_in.texCoords).rgb;
		hdrColor = vec3(1.0f) - exp(-hdrColor * exposure);
		fs_outColor = vec4(hdrColor, 1.0f);	
	}
}
