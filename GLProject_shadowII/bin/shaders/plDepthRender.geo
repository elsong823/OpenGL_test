#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 matLightMatrices[6];
out vec4 fragPos;

void main()
{
	for(int face = 0; face < 6; ++face)
	{
		gl_Layer = face;
		for(int i = 0; i < 3; ++i)
		{
			//把世界坐标传递到fragment shader中
			fragPos = gl_in[i].gl_Position;
			//用每一面的光空间矩阵*世界矩阵
			gl_Position = matLightMatrices[face] * gl_in[i].gl_Position;
			//gl_Position = matLightMatrices[i] * gl_in[i].gl_Position;
			EmitVertex();
		}
		EndPrimitive();
	}
}