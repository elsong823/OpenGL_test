#version 330 core

layout (location = 0) out vec4 gPositionDepth;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;

in VS_OUT
{
	vec2 texCoords;
	vec3 fragPos;
	vec3 normal;
}fs_in;

//与相机矩阵相同
const float NEAR = 0.1f;
const float FAR = 50.0f;
float CalcDepth(float _depth);

void main()
{
	//保存位置
	gPositionDepth.xyz = fs_in.fragPos;
	gPositionDepth.w = CalcDepth(gl_FragCoord.z);
	//保存法线
	gNormal = normalize(fs_in.normal);
	//返回灰白色
	gAlbedo = vec3(0.95);
}

float CalcDepth(float _depth)
{
	//转换到NDC -1~1中
	float z = _depth * 2.0 - 1.0;
	return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));
	//如果没有使用GL_FLOAT这种存储类型，则还需要将结果除以FAR以转换到0~1的范围中！
	//return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR)) / FAR;
}