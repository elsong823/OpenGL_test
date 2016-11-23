#include "Utility.h"
#include "MyShaderLoader.h"

CMyShaderLoader::CMyShaderLoader()
{
}

CMyShaderLoader::~CMyShaderLoader()
{
}

GLuint CMyShaderLoader::CreateProgram(GLchar * _verShaderPath, GLchar * _fragShaderPath)
{
	GLchar * _verSource = LoadFile(_verShaderPath);
	auto _verShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(_verShader, 1, &_verSource, nullptr);
	DELETE_ARRAY(_verSource);
	glCompileShader(_verShader);
	if (!CheckShader(_verShader))
	{
		return -1;
	}

	GLchar * _fragSource = LoadFile(_fragShaderPath);
	auto _fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(_fragShader, 1, &_fragSource, nullptr);
	DELETE_ARRAY(_fragSource);
	glCompileShader(_fragShader);
	if (!CheckShader(_fragShader))
	{
		glDeleteShader(_verShader);
		return -1;
	}

	auto _program = glCreateProgram();
	glAttachShader(_program, _verShader);
	glAttachShader(_program, _fragShader);
	glLinkProgram(_program);
	glDeleteShader(_verShader);
	glDeleteShader(_fragShader);
	if (!CheckProgram(_program))
	{
		return -1;
	}
	//Save program to store
	m_programs.push_back(_program);
	return _program;
}

GLchar * CMyShaderLoader::LoadFile(GLchar * _filePath)
{
	int _fileLength = 0;
	FILE * _file = nullptr;
	GLchar * _source = nullptr;
	fopen_s(&_file, _filePath, "r");
	fseek(_file, 0, SEEK_END);
	_fileLength = ftell(_file);
	rewind(_file);
	_source = new GLchar[_fileLength + 1];
	fread(_source, _fileLength, 1, _file);
	fclose(_file);
	return _source;
}

GLboolean CMyShaderLoader::CheckShader(GLuint _shader)
{
	GLint _success = 0;
	glGetShaderiv(_shader, GL_COMPILE_STATUS, &_success);
	if (!_success)
	{
		GLchar _infoLog[512] = {};
		glGetShaderInfoLog(_shader, sizeof(_infoLog), nullptr, _infoLog);
		printf_s("Create shader failed\n%s\n", _infoLog);
		return false;
	}
	return true;
}

GLboolean CMyShaderLoader::CheckProgram(GLuint _program)
{
	GLint _success = 0;
	glGetProgramiv(_program, GL_LINK_STATUS, &_success);
	if (!_success)
	{
		GLchar _infoLog[512] = {};
		glGetProgramInfoLog(_program, sizeof(_infoLog), nullptr, _infoLog);
		printf_s("Link program failed\n%s\n", _infoLog);
		return false;
	}
	return true;
}

void CMyShaderLoader::DeleteProgram(GLuint _program)
{
	for (auto _iter = m_programs.begin();
		_iter != m_programs.end();
		++_iter)
	{
		if (*_iter == _program)
		{
			m_programs.erase(_iter);
			break;
		}
	}
	glDeleteProgram(_program);
}