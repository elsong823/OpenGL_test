#version 330 core

layout (points) in;
layout (line_strip, max_vertices = 2) out;

in VS_OUT
{
	vec4 lightStart;
	vec4 lightEnd;
}ge_in[];

void main()
{
	gl_Position = ge_in[0].lightStart;
	EmitVertex();
	
	gl_Position = ge_in[0].lightEnd;
	EmitVertex();
	
	EndPrimitive();
}