
#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 2) out;

in VS_OUT
{
	vec3 normal;
}gs_in[];

const float magnitude = 0.4f;

void GenLine(int idx);

void main()
{
	for(int i = 0; i < 3; ++i)
	{
		GenLine(i);
	}
}
void GenLine(int idx)
{
	gl_Position = gl_in[idx].gl_Position;
	EmitVertex();
	
	gl_Position = gl_in[idx].gl_Position + vec4(gs_in[idx].normal, 0.0f) * magnitude;
	EmitVertex();
	
	EndPrimitive();
}
