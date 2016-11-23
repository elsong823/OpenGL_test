#include "Utility.h"
#include "MyShader.h"


CMyShader::CMyShader()
{
}

CMyShader::~CMyShader()
{
}

GLuint CMyShader::CreateShaderProgram(GLchar * _vert, GLchar * frag)
{
	auto vertSource = this->LoadFile(_vert);
	auto vertShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertShader, 1, &vertSource, nullptr);
	glCompileShader(vertShader);
	this->CheckShader(vertShader);

	auto fragSource = this->LoadFile(frag);
	auto fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShader, 1, &fragSource, nullptr);
	glCompileShader(fragShader);
	this->CheckShader(fragShader);

	auto program = glCreateProgram();
	glAttachShader(program, vertShader);
	glAttachShader(program, fragShader);
	glLinkProgram(program);
	this->CheckProgram(program);

	delete[] vertSource;
	delete[] fragSource;
	glDeleteShader(vertShader);
	glDeleteShader(fragShader);
	return program;
}	

GLuint CMyShader::CreateShaderProgram(GLchar * _vert, GLchar * _geo, GLchar * _frag)
{
	GLchar * vertSource;
	GLchar * geoSource;
	GLchar * fragSource;

	vertSource = LoadFile(_vert);
	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertShader, 1, &vertSource, nullptr);
	glCompileShader(vertShader);
	CheckShader(vertShader);

	geoSource = LoadFile(_geo);
	GLuint geoShader = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geoShader, 1, &geoSource, nullptr);
	glCompileShader(geoShader);
	CheckShader(geoShader);

	fragSource = LoadFile(_frag);
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShader, 1, &fragSource, nullptr);
	glCompileShader(fragShader);
	CheckShader(fragShader);

	GLuint program = glCreateProgram();
	glAttachShader(program, vertShader);
	glAttachShader(program, geoShader);
	glAttachShader(program, fragShader);
	glLinkProgram(program);
	CheckProgram(program);

	glDeleteShader(vertShader);
	glDeleteShader(geoShader);
	glDeleteShader(fragShader);
	delete[] vertSource;
	delete[] geoSource;
	delete[] fragSource;

	return program;
}

void CMyShader::CheckShader(GLuint _shader)
{
	int success = 0;
	glGetShaderiv(_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512] = {};
		glGetShaderInfoLog(_shader, sizeof(infoLog), nullptr, infoLog);
		printf_s("Create shader failed\n%s\n",infoLog);
	}
}

void CMyShader::CheckProgram(GLuint _program)
{
	int success = 0;
	glGetProgramiv(_program, GL_LINK_STATUS, &success);
	if (!success)
	{
		char infoLog[512] = {};
		glGetShaderInfoLog(_program, sizeof(infoLog), nullptr, infoLog);
		printf_s("Create program failed\n%s\n", infoLog);
	}
}

GLchar * CMyShader::LoadFile(GLchar * _filePath)
{
	FILE * file = nullptr;
	int length = 0;
	GLchar * source = nullptr;
	fopen_s(&file, _filePath, "r");
	fseek(file, 0, SEEK_END);
	length = ftell(file);
	rewind(file);
	source = new GLchar[length + 1];
	fread(source, length, 1, file);
	source[length] = '\0';
	fclose(file);
	return source;
}