#version 330 core

in vec2 outTexCoord;
in vec3 outColor;

out vec4 color;
uniform sampler2D tex1;
uniform sampler2D tex2;

void main()
{
	vec4 _colorTemp = texture(tex1, outTexCoord);
	_colorTemp.r = (_colorTemp.r + _colorTemp.g + _colorTemp.b) / 3.0f;
	_colorTemp.g = _colorTemp.r;
	_colorTemp.b = _colorTemp.r;
	color = mix(_colorTemp, texture(tex2, outTexCoord), 0.2f);
}