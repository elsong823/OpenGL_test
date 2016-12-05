
#version 330 core

layout (points) in;
layout (line_strip, max_vertices = 2) out;

in VS_OUT
{
	vec4 start;
	vec4 end;
}gs_in[];

void main()
{
	gl_Position = gs_in[0].start;
	EmitVertex();
	
	gl_Position = gs_in[0].end;
	EmitVertex();
	
	EndPrimitive();
}

