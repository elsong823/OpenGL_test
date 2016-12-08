#version 330 core

layout(location = 0) in vec4 vertex;
out vec2 texCoords;

uniform mat4 matProjection;

void main()
{
	gl_Position = matProjection * vec4(vertex.xy, 0.0f, 1.0f);
	texCoords = vertex.zw;
}