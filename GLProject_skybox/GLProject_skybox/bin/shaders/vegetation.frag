
#version 330 core

uniform sampler2D fragTexture;
varying vec2 fragTexCoord;
out vec4 o_outColor;

void main()
{	
	vec4 texColor = texture(fragTexture, fragTexCoord);	
	if(texColor.a < 0.01f)
		discard;
	o_outColor = texColor;
}