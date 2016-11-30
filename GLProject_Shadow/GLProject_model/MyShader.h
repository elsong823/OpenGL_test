#pragma once

class CMyShader
{
	SINGLETON(CMyShader);
private:
	CMyShader();
public:
	~CMyShader();
	GLuint CreateShaderProgram(GLchar * _vert, GLchar * frag);
	GLuint CreateShaderProgram(GLchar * _vert, GLchar * _geo, GLchar * frag);

private:
	void CheckShader(GLuint _shader);
	void CheckProgram(GLuint _program);
	GLchar * LoadFile(GLchar * _filePath);
};

