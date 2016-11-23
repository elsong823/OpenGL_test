#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoord;

out vec3 outColor;
out vec2 outTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
	outColor = vec3(1.0f, 1.0f, 0.0f);
	outTexCoord = vec2(texCoord.x, 1.0f - texCoord.y);
} 