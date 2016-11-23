#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec2 outCoord[];
out vec2 fragCoord;

uniform float timer;

//获得法线
vec3 GetNormal();
vec4 Explosion(vec4 point, vec3 normal);

void main()
{
	vec3 normal = GetNormal();
	for(int i = 0; i < 3; ++i)
	{
		gl_Position = Explosion(gl_in[i].gl_Position, normal);
		fragCoord = outCoord[i];
		EmitVertex();
	}
	EndPrimitive();
}

vec3 GetNormal()
{
	vec3 lineA = vec3(gl_in[1].gl_Position - gl_in[0].gl_Position);
	vec3 lineB = vec3(gl_in[2].gl_Position - gl_in[0].gl_Position);
	return normalize(cross(lineB, lineA));
}

vec4 Explosion(vec4 point, vec3 normal)
{
	float x = (sin(timer) + 1.0f)  * 0.5f;
	vec3 _distance = normal * 1.0f * x;
	return point + vec4(_distance, 0.0f);
}