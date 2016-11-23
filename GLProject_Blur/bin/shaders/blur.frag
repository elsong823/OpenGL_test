#version 330 core

uniform sampler2D tex;
uniform int type;
uniform int range;
in vec2 fragTexCoord;

out vec4 outColor;

void main()
{
	if(type == 0)
	{
		outColor = texture2D(tex, fragTexCoord);
		return;
	}
	
	float offset = 1.0 / 800;
	//1:horizontal
	//2:vertical
	if(type == 2)
	{
		offset = 1.0 / 600;
	}
	offset *= range;
	float gaussian[7] = float[](0.0205, 0.0855, 0.232, 0.324, 0.232, 0.0855, 0.0205);
	float offsets[7] = float[](-3*offset, -2*offset, -1*offset, 0, 1*offset, 2*offset, 3*offset);
	vec4 color = vec4(0.0f);
	vec2 newCoord;
	for(int i = 0; i < 7; ++i)
	{
		if(type == 1)
		{
			newCoord.s = fragTexCoord.s + offsets[i];
			newCoord.t = fragTexCoord.t;
		}
		else if(type == 2)
		{			
			newCoord.s = fragTexCoord.s;
			newCoord.t = fragTexCoord.t + offsets[i];
		}
		color += texture2D(tex, newCoord) * gaussian[i];
	}
	outColor =  vec4(color.rgb, 1.0f);
}