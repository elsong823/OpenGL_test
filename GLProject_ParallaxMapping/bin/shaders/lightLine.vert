
#version 330 core

uniform mat4 matView;
uniform mat4 matProjection;

uniform vec3 start;
uniform vec3 end;

out VS_OUT
{
	vec4 start;
	vec4 end;
}vs_out;

void main()
{
	vs_out.start = matProjection * matView * vec4(start,1.0f);
	vs_out.end = matProjection * matView * vec4(end,1.0f);
}
