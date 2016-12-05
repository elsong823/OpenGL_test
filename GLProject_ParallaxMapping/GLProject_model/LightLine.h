#pragma once

class CMyCamera;

class CLightLine
{
public:
	CLightLine(glm::vec3 _start, glm::vec3 _end, CMyCamera * _camera);
	~CLightLine();
	
public:
	void Draw();

private:
	CLightLine();

private:
	void SetupLightLine();

private:
	static GLuint VAO;
	static GLuint VBO;
	static GLuint shaderProgram;
	static CMyCamera * camera;
	glm::vec3 m_start;
	glm::vec3 m_end;
};

