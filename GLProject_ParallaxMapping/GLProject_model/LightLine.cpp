#include "Utility.h"
#include "MyCamera.h"
#include "MyShader.h"
#include "LightLine.h"

GLuint CLightLine::VAO = 0;
GLuint CLightLine::VBO = 0;
GLuint CLightLine::shaderProgram = 0;
CMyCamera * CLightLine::camera = nullptr;

CLightLine::CLightLine()
{
}

CLightLine::CLightLine(glm::vec3 _start, glm::vec3 _end, CMyCamera * _camera)
{
	m_start = _start;
	m_end = _end;
	camera = _camera;
	this->SetupLightLine();
}

CLightLine::~CLightLine()
{
}

void CLightLine::Draw()
{
	if (!camera || !VAO)
	{
		return;
	}
	glUseProgram(shaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "matView"), 1, GL_FALSE, glm::value_ptr(*camera->GetView()));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "matProjection"), 1, GL_FALSE, glm::value_ptr(*camera->GetProjection()));
	glUniform3fv(glGetUniformLocation(shaderProgram, "start"), 1, glm::value_ptr(m_start));
	glUniform3fv(glGetUniformLocation(shaderProgram, "end"), 1, glm::value_ptr(m_end));

	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, 1);
	glBindVertexArray(0);
}

void CLightLine::SetupLightLine()
{
	if (!VAO)
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* 3, nullptr, GL_STATIC_DRAW);
		glBindVertexArray(0);
		shaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/lightLine.vert","../bin/shaders/lightLine.geo","../bin/shaders/lightLine.frag");
	}
}
