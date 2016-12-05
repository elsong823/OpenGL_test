#version 330 core

layout (points) in;
layout (line_strip, max_vertices = 2) out;

in VS_OUT
{
	vec4 lightStart;
	vec4 lightEnd;
}gs_in[];

void main()
{
	//光的位置
	gl_Position = gs_in[0].lightStart;
	EmitVertex();
	
	//光线
	gl_Position = gs_in[0].lightEnd;
	EmitVertex();
	
	EndPrimitive();
}  