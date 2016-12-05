#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;

struct SpotLight
{
	vec3 position;
	vec3 dir;
	float cutoff;
	float outCutoff;
	float constant;
	float linear;
	float quadratic;
};

layout (std140) uniform Matrices
{
	mat4 matView;
	mat4 matProjection;
};

out VS_OUT
{
	vec2 texCoord;
	mat3 matTBN;
	vec3 tangentLightPos;
	vec3 tangentViewPos;
	vec3 tangentFragPos;
	vec3 tangentSpotDir;
}vs_out;

uniform mat4 matModel;
uniform vec3 tangent;
uniform vec3 bitangent;
uniform SpotLight spotLight;
uniform vec3 viewPos;
out vec3 fragPos;
out vec3 fragNormal;

void main()
{
	gl_Position = matProjection * matView * matModel * vec4(position, 1.0f);
	vs_out.texCoord = texCoord;
	mat3 matNormal = transpose(inverse(mat3(matModel)));
	fragPos = vec3(matModel * vec4(position, 1.0f));
	fragNormal = matNormal * normal;
	//Create TBN matrix
	vec3 T = normalize(vec3(matNormal * tangent));
	vec3 B = normalize(vec3(matNormal * bitangent));
	vec3 N = normalize(vec3(matNormal * cross(tangent, bitangent)));
	//方法1：传TBN给片段着色器，用以变化法线
	vs_out.matTBN = mat3(T, B, N);
	//方法2：传被TBN转置矩阵处理后的点给fragment shader
	//应该转逆矩阵，但是正交矩阵的转置矩阵 == 逆矩阵，而转置矩阵计算量更小，所以使用转置矩阵
	vs_out.matTBN = transpose(vs_out.matTBN);
	vs_out.tangentFragPos = vs_out.matTBN * fragPos;
	vs_out.tangentLightPos = vs_out.matTBN * spotLight.position;
	vs_out.tangentViewPos = vs_out.matTBN * viewPos;
	vs_out.tangentSpotDir = transpose(inverse(vs_out.matTBN)) * spotLight.dir;
}