#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 5) out;


in vec3 outColor[];
out vec3 heheColor;

void BuildHouce(vec4 pos)
{
	gl_Position = pos + vec4(-0.2f, -0.2f, 0.0f, 0.0f);
	EmitVertex();
	
	gl_Position = pos + vec4( 0.2f, -0.2f, 0.0f, 0.0f);
	EmitVertex();
	
	gl_Position = pos + vec4(-0.2f,  0.2f, 0.0f, 0.0f);
	EmitVertex();
	
	gl_Position = pos + vec4( 0.2f,  0.2f, 0.0f, 0.0f);
	EmitVertex();

	gl_Position = pos + vec4( 0.0f,  0.4f, 0.0f, 0.0f);
	heheColor = vec3(1.0f);
	EmitVertex();
}

void main()
{
	heheColor = outColor[0];
	BuildHouce(gl_in[0].gl_Position);
	EndPrimitive();
}
