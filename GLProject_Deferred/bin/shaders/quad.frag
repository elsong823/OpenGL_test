#version 330 core

layout (location = 0) out vec4 FragColor;

in vec2 fragTexCoord;
uniform sampler2D texSource;
uniform bool showAlbedo;
uniform bool showSpec;

void main()
{
	if(showAlbedo)
	{
		vec3 pixel = texture2D(texSource, fragTexCoord).rgb;
		FragColor = vec4(pixel, 1.0f);
	}
	else if(showSpec)
	{
		float spec = texture2D(texSource, fragTexCoord).a;
		FragColor = vec4(spec, spec, spec, 1.0f);
	}
	else
	{
		FragColor = texture2D(texSource, fragTexCoord);
	}
}

