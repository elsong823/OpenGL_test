
#version 330 core

varying vec3 v_fragColor;
out vec4 o_outColor;

void main()
{
	o_outColor = vec4(v_fragColor, 1.0f);
}