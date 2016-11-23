#pragma once

class CMyShaderLoader
{
	SINGLETON(CMyShaderLoader);
private:
	CMyShaderLoader();

public:
	~CMyShaderLoader();

public:
	GLuint CreateProgram(GLchar * _verShaderPath, GLchar * _fragShaderPath);
	void DeleteProgram(GLuint _program);

private:
	GLchar * LoadFile(GLchar * _filePath);
	GLboolean CheckShader(GLuint _shader);
	GLboolean CheckProgram(GLuint _program);

private:
	list<GLuint> m_programs;
};

