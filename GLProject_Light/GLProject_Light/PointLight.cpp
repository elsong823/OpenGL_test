#include "Utility.h"
#include "MyShaderLoader.h"
#include "MyCamera.h"
#include "PointLight.h"

GLuint CPointLight::VAO = 0;
GLuint CPointLight::VBO = 0;

CPointLight::CPointLight()
{
}

CPointLight::~CPointLight()
{
}

CPointLight::CPointLight(CMyCamera * _camera, glm::vec3 _pos, float _linear, float _quadratic, glm::vec3 _lightColor)
:m_position(_pos),
m_linear(_linear),
m_quadratic(_quadratic),
m_lightColor(_lightColor),
m_camera(_camera),
m_const(POINT_LIGHT_CONST)
{
	m_matView = m_camera->GetView();
	m_matProjection = m_camera->GetProjection();
	m_matModel = glm::translate(m_matModel, m_position);
	m_matModel = glm::scale(m_matModel, glm::vec3(0.3f));
	this->Init();
}

void CPointLight::Draw()
{
	glUseProgram(m_shaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(m_matModel));
	glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "matView"), 1, GL_FALSE, glm::value_ptr(*m_matView));
	glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "matProjection"), 1, GL_FALSE, glm::value_ptr(*m_matProjection));

	glBindVertexArray(CPointLight::VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void CPointLight::Init()
{
	m_shaderProgram = CMyShaderLoader::GetInstance()->CreateProgram("../bin/Shaders/light.vert", "../bin/Shaders/light.frag");
	if (!CPointLight::VAO)
	{
		GLfloat _vertices[] = {
			// Positions 
			-0.2f, -0.2f, -0.2f,
			0.2f, -0.2f, -0.2f, 
			0.2f, 0.2f, -0.2f, 
			0.2f, 0.2f, -0.2f, 
			-0.2f, 0.2f, -0.2f, 
			-0.2f, -0.2f, -0.2f,

			-0.2f, -0.2f, 0.2f,
			0.2f, -0.2f, 0.2f, 
			0.2f, 0.2f, 0.2f,
			0.2f, 0.2f, 0.2f,
			-0.2f, 0.2f, 0.2f, 
			-0.2f, -0.2f, 0.2f,

			-0.2f, 0.2f, 0.2f, 
			-0.2f, 0.2f, -0.2f,
			-0.2f, -0.2f, -0.2f,
			-0.2f, -0.2f, -0.2f,
			-0.2f, -0.2f, 0.2f,
			-0.2f, 0.2f, 0.2f, 

			0.2f, 0.2f, 0.2f, 
			0.2f, 0.2f, -0.2f, 
			0.2f, -0.2f, -0.2f,
			0.2f, -0.2f, -0.2f,
			0.2f, -0.2f, 0.2f, 
			0.2f, 0.2f, 0.2f, 

			-0.2f, -0.2f, -0.2f,
			0.2f, -0.2f, -0.2f,
			0.2f, -0.2f, 0.2f, 
			0.2f, -0.2f, 0.2f, 
			-0.2f, -0.2f, 0.2f,
			-0.2f, -0.2f, -0.2f,

			-0.2f, 0.2f, -0.2f,
			0.2f, 0.2f, -0.2f, 
			0.2f, 0.2f, 0.2f, 
			0.2f, 0.2f, 0.2f, 
			-0.2f, 0.2f, 0.2f, 
			-0.2f, 0.2f, -0.2f,
		};

		glGenVertexArrays(1, &CPointLight::VAO);
		glGenBuffers(1, &CPointLight::VBO);
		glBindVertexArray(CPointLight::VAO);
		glBindBuffer(GL_ARRAY_BUFFER, CPointLight::VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(_vertices), _vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*3, (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}