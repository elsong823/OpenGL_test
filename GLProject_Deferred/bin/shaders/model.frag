
#version 330 core

layout (location = 0) out vec3 gPosition;	//位置
layout (location = 1) out vec3 gNormal;		//法线	
layout (location = 2) out vec4 gAlbedoSpec;	//颜色、高光

uniform sampler2D tex_diffuse1;
uniform sampler2D tex_specular1;

in VS_OUT
{
	vec3 fragPos;
	vec3 fragNormal;
	vec2 fragCoords;
}fs_in;

void main()
{
	//保存世界坐标
	gPosition = fs_in.fragPos;
	//保存世界空间内的法线
	gNormal = normalize(fs_in.fragNormal);
	//保存漫反射率
	gAlbedoSpec.rgb = texture2D(tex_diffuse1, fs_in.fragCoords).rgb;
	//保存高光
	gAlbedoSpec.a = texture2D(tex_specular1, fs_in.fragCoords).r;
}