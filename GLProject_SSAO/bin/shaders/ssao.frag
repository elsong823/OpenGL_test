#version 330 core

//由于是GL_RED格式，因此这里返回一个float
layout (location = 0) out float FragColor;

in vec2 fragTexCoords;

//位置
uniform sampler2D gPositionDepth;
//法线
uniform sampler2D gNormal;
//随机噪声（就是那个旋转）
uniform sampler2D texNoise;

//由于记录随机旋转的纹理尺寸仅为4x4，而屏幕大小为800 x 600，这个纹理无法平铺
//因为uv范围总在0~1之间，为了进行全幅取样，需要缩放取样uv
//此前也设置了采样wrap为GL_REPEA
//计算缩放比例
const vec2 noiseScale = vec2(800.0 / 4.0, 600.0 / 4.0);

//64个采样点
uniform vec3 samples[64];
uniform mat4 matProjection;

const int kernelSize = 64;	//64个采样点
const float radius = 1.0;	//采样半径

void main()
{
	//注意这两个都在观察者空间
	vec3 fragPos = texture2D(gPositionDepth, fragTexCoords).xyz;
	vec3 normal = normalize(texture2D(gNormal, fragTexCoords).rgb);
	//这个是在切线空间
	vec3 randomVec = texture2D(texNoise, fragTexCoords * noiseScale).xyz;
	//使用Gramm-schmid创建TBN矩阵,这个矩阵将从切线空间转换为观察者空间
	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);
	
	//计算遮蔽值
	float occlusion = 0.0;
	for(int i = 0; i < kernelSize; ++i)
	{	
		//计算采样点位置-并转换为观察者空间
		vec3 samplePos = TBN * samples[i];	//这个计算的是移动的向量
		samplePos = fragPos + samplePos * radius;	//计算完才是位置
		//变换到ndc中
		vec4 offset = vec4(samplePos, 1.0f);
		offset = matProjection * offset;
		offset.xyz /= offset.w;					//透视除法，得到的结果是-1~1
		offset.xyz = offset.xyz * 0.5 + 0.5;	//变换到0~1中，可以uv采样了
		//获取这个点的深度值
		float sampleDepth = -texture2D(gPositionDepth, offset.xy).w;
		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
		occlusion += (sampleDepth >= samplePos.z ? 1.0 : 0.0) * rangeCheck;
	}
	occlusion = 1.0 - (occlusion / kernelSize);
	FragColor = occlusion;
}	