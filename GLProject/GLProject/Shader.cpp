#include "Shader.h"	

CShader::CShader()
{
}

CShader::CShader(GLchar * _vertexSourcePath, GLchar * _fragmentSourcePath)
{
	Init(_vertexSourcePath, _fragmentSourcePath);
}

CShader::~CShader()
{
}

void CShader::Init(GLchar * _vertexSourcePath, GLchar * _fragmentSourcePath)
{
	//Load file
	auto _vertexSource = LoadFile(_vertexSourcePath);
	auto _fragmentSource = LoadFile(_fragmentSourcePath);
	//CreateShader
	auto _vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(_vertexShader, 1, &_vertexSource, nullptr);
	glCompileShader(_vertexShader);
	CheckShader(_vertexShader);

	auto _fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(_fragmentShader, 1, &_fragmentSource, nullptr);
	glCompileShader(_fragmentShader);
	CheckShader(_fragmentShader);

	//Create Program
	m_program = glCreateProgram();
	glAttachShader(m_program, _vertexShader);
	glAttachShader(m_program, _fragmentShader);
	glLinkProgram(m_program);
	CheckProgram(m_program);

	delete[] _vertexSource;
	delete[] _fragmentSource;
	glDeleteShader(_vertexShader);
	glDeleteShader(_fragmentShader);
}

void CShader::UseProgram()
{
	glUseProgram(m_program);
}

GLchar * CShader::LoadFile(GLchar * _path)
{
	FILE * _file = nullptr;
	fopen_s(&_file, _path, "r");
	//Get file length
	GLuint _size = 0;
	fseek(_file, 0, SEEK_END);
	_size = ftell(_file);
	rewind(_file);
	auto _source = new GLchar[_size + 1];
	fread_s(_source, _size, _size, 1, _file);
	_source[_size] = '\0';
	fclose(_file);
	return _source;
}

void CShader::CheckShader(GLuint _shader)
{
	GLint _success = 0;
	glGetShaderiv(_shader, GL_COMPILE_STATUS, &_success);
	if (!_success)
	{
		GLchar _infoLog[512] = {};
		glGetShaderInfoLog(_shader, sizeof(_infoLog), nullptr, _infoLog);
		printf_s("Create shader failed\n%s", _infoLog);
	}
}

void CShader::CheckProgram(GLuint _program)
{
	GLint _success = 0;
	glGetProgramiv(_program, GL_LINK_STATUS, &_success);
	if (!_success)
	{
		GLchar _infoLog[512] = {};
		glGetProgramInfoLog(_program, sizeof(_infoLog), nullptr, _infoLog);
		printf_s("Create program failed\n%s", _infoLog);
	}
}