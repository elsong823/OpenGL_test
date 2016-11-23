
#version 330 core

out vec4 color;

varying VS_OUT
{
	vec3 color;
}fs_in;

void main()
{
	color = vec4(fs_in.color, 1.0f);
}