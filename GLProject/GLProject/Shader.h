#include "Utility.h"

class CShader
{
private:
	CShader();

public:
	CShader(GLchar * _vertexSourcePath, GLchar * _fragmentSourcePath);
	~CShader();

public:
	void Init(GLchar * _vertexSourcePath, GLchar * _fragmentSourcePath);
	GLuint GetProgram(){ return m_program; }
	void UseProgram();

private:
	GLchar * LoadFile(GLchar * _path);
	void CheckShader(GLuint _shader);
	void CheckProgram(GLuint _program); 
	
private:
	GLuint m_program;

};