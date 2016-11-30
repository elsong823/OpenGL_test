#version 330 core

out vec4 color;
uniform sampler2D texWall;
uniform samplerCube texShadow;
uniform vec3 lightPos;
uniform float farPlane;

in VS_OUT
{
	vec2 texCoord;
	vec3 fragPos;
}fs_in;

float CalcShadow();

void main()
{
	color = texture2D(texWall, fs_in.texCoord) * 0.6f * (1.0f - CalcShadow());
	vec3 fragToLight = fs_in.fragPos - lightPos;
	//color = vec4(vec3(texture(texShadow, fragToLight).r), 1.0f);
}

float CalcShadow()
{
	//float offset = 0.01f;
	//float samples = 4.0f;
	//float bias = 0.05f;
	//float shadow = 0.0f;
	//vec3 fragToLight = fs_in.fragPos - lightPos;
	//float curDepth = length(fragToLight);
	//for(float x = -offset; x < offset; x += offset / (samples * 0.5f))
	//{
	//	for(float y = -offset; y < offset; y += offset / (samples * 0.5f))
	//	{
	//		for(float z = -offset; z < offset; z += offset / (samples * 0.5f))
	//		{
	//			float depthInMap = texture(texShadow, normalize(fragToLight) + vec3(x,y,z)).r * farPlane;
	//			if(curDepth - bias > depthInMap)
	//			{
	//				shadow += 1.0f;
	//			}
	//		}	
	//	}
	//}
	////采样次数过多，没必要，可参考newBox实现
	//return shadow / (samples * samples * samples);
	
		//固定位置采样
	vec3 sampleOffsetDirections[20] = vec3[]
	(
	   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
	   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
	   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
	   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
	   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
	);
	float bias = 0.05f;
	vec3 fragToLight = fs_in.fragPos - lightPos;
	float curDepth = length(fragToLight);
	//float diskRadius = 0.05f;
	float diskRadius = (1.0f + (curDepth / farPlane)) / farPlane;
	float shadow = 0.0f;
	for(int i = 0; i < 20; ++i)
	{
		float depthInMap = texture(texShadow, fragToLight + diskRadius * sampleOffsetDirections[i]).r * farPlane;
		if(curDepth - bias > depthInMap)
		{
			shadow += 1.0f;
		}
	}
	return shadow / 20.0f;
}