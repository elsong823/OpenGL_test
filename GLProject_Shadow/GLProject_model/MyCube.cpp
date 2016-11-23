#include "Utility.h"
#include "MyCamera.h"
#include "MyCube.h"

GLuint CMyCube::VAO = 0;
GLuint CMyCube::VBO = 0;
const glm::vec3 * CMyCube::cameraPos = nullptr;

CMyCube::CMyCube()
:m_dirty(true),
m_rotateAxis(1.0f, 1.0f, 1.0f),
m_scale(1.0f, 1.0f, 1.0f)
{
	this->Init();
}

CMyCube::~CMyCube()
{
}

void CMyCube::Init()
{
	if (!CMyCube::VAO)
	{

		GLfloat _vertices[] = {
			// Positions           // Normals           // Texture Coords
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
			0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
			0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
			0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
			-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
			0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
			0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
			-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

			-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
			-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

			0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
			0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
			0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

			-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
			0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
			0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
			0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
			-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
		};

		glGenVertexArrays(1, &CMyCube::VAO);
		glGenBuffers(1, &CMyCube::VBO);
		glBindVertexArray(CMyCube::VAO);
		glBindBuffer(GL_ARRAY_BUFFER, CMyCube::VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(_vertices), _vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 8, (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 8, (GLvoid*)(sizeof(GLfloat)* 3));
		glEnableVertexAttribArray(2);		
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 8, (GLvoid*)(sizeof(GLfloat)* 6));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}

void CMyCube::SetCamera(CMyCamera * _camera)
{
	m_camera = _camera;
	m_matView = m_camera->GetView();
	m_matProjection = m_camera->GetProjection();
	cameraPos = m_camera->GetPosition();
}

void CMyCube::Update(GLfloat _dt)
{
	if (m_dirty)
	{
		m_matModel = glm::mat4();
		m_matModel = glm::translate(m_matModel, m_position);
		if (m_radian > 0.01f)
			m_matModel = glm::rotate(m_matModel, m_radian, m_rotateAxis);
		m_matModel = glm::scale(m_matModel, m_scale);
		m_dirty = false;
	}
}

void CMyCube::Draw(GLuint _shaderProgram)
{
	m_shaderProgram = _shaderProgram;
	glUseProgram(m_shaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(m_matModel));
	
	SubsetDraw();

	glBindVertexArray(CMyCube::VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void CMyCube::SetPosition(glm::vec3 _pos)
{
	m_position = _pos;
	m_dirty = true;
}

void CMyCube::SetRotation(glm::vec3 _axis, GLfloat _degree)
{
	m_rotateAxis = _axis;
	m_radian = glm::radians(_degree);
	m_dirty = true;
}

void CMyCube::SetScale(glm::vec3 _scale)
{
	m_scale = _scale;
	m_dirty = true;
}
