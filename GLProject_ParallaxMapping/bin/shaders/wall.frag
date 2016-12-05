
#version 330 core

uniform sampler2D texDiffuse;
uniform sampler2D texNormal;
uniform sampler2D texParallax;

in VS_OUT
{
	vec2 texCoord;
	vec3 eyePos;
	vec3 lightDir;
	vec3 fragPos;
}fs_in;

uniform int useParallax;
uniform float height_scale;
out vec4 fs_outColor;

vec2 ParallaxMapping(vec2 texCoord, vec3 viewDir);

void main()
{
	//光
	vec3 light = vec3(1.0f, 1.0f, 1.0f);
	vec3 lightDir = normalize(fs_in.lightDir);
	vec3 viewDir = normalize(fs_in.fragPos - fs_in.eyePos);
	//计算uv
	vec2 texCoord = fs_in.texCoord;
	if(useParallax > 0)
	{
		texCoord = ParallaxMapping(fs_in.texCoord, -viewDir);
	}
	if(texCoord.x > 1.0f
		|| texCoord.y > 1.0f
		|| texCoord.x < 0.0f
		|| texCoord.y < 0.0f)
	{
		discard;
	}
	vec3 pixelDiffuse = texture2D(texDiffuse, texCoord).rgb;
	vec3 pixelNormal = texture2D(texNormal, texCoord).rgb;
	pixelNormal = pixelNormal * 2.0f - 1.0f;
	
	//ambient
	vec3 ambient = pixelDiffuse * light * 0.2f;
	
	//diffuse
	float diff = max(dot(-lightDir, pixelNormal), 0.0f);
	vec3 diffuse = pixelDiffuse * light * diff * 0.8f;
	
	//specular
	vec3 halfway = normalize(-lightDir + -viewDir);
	float spec = pow(max(dot(halfway, pixelNormal), 0.0f), 32.0f);
	vec3 specular = pixelDiffuse * light * spec * 0.0f;
	fs_outColor = vec4(ambient + diffuse + specular, 1.0f);
}

vec2 ParallaxMapping(vec2 texCoord, vec3 viewDir)
{ 
	//普通视差
    //float height =  texture(texParallax, texCoord).r;     
    //return texCoord - viewDir.xy / viewDir.z * (height * height_scale);      
	
	
	//陡峭视差
	//几层
	//const float numLayers = 30.0f;
	//const float numLayers = 100.0f;
	//根据夹角动态判断
	float minLayers = 8.0f;
	float maxLayers = 20.0f;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  //mix(x, y, a): x, y的线性混叠， x(1-a) + y*a;
	//每层的高度
	float layerDepth = 1.0 / numLayers;
	//每层的纹理坐标
    vec2 P = viewDir.xy / viewDir.z * height_scale; 
    vec2 deltaTexCoords = P / numLayers;
	//当前层深度
	vec2 curTexCoords = texCoord;
	float curDepthMapValue = texture2D(texParallax, curTexCoords).r;
	float curDepth = 0.0f;
	while(curDepth < curDepthMapValue)
	{
		curTexCoords -= deltaTexCoords;
		curDepthMapValue = texture2D(texParallax, curTexCoords).r;
		curDepth += layerDepth;
	}
	
	//错误
	//return texCoord - curTexCoords;
	
	//为了更精确，计算权重
	vec2 preCoords = curTexCoords + deltaTexCoords;
	float preDepthInMap = texture2D(texParallax, preCoords).r;
	float preDepthInLayer = curDepth - layerDepth;
	float weight = (curDepth - curDepthMapValue) / (curDepth - curDepthMapValue + preDepthInMap - preDepthInLayer);
	
	//注意此处：
	//上次的偏移坐标 * 本次的权重：因为本次偏移越大，上次所占比例约大，本次偏移越小，上次偏移所占越小
	//偏移指的是理论深度和depthInMap的差距，如偏移为0，则上次乘完就为0了！
	vec2 finalCoords = preCoords * weight + curTexCoords * (1.0f - weight);
	return finalCoords;
	
	//vec2 prevTexCoords = curTexCoords + deltaTexCoords;
    //
    //// get depth after and before collision for linear interpolation
    //float afterDepth  = curDepthMapValue - curDepth;
    //float beforeDepth = texture(texParallax, prevTexCoords).r - curDepth + layerDepth;
	//float x = curDepthMapValue - texture(texParallax, prevTexCoords).r - layerDepth;
    //// interpolation of texture coordinates
    //float weight = afterDepth / (afterDepth - beforeDepth);
    //vec2 finalTexCoords = prevTexCoords * weight + curTexCoords * (1.0 - weight);

    //return finalTexCoords;
}