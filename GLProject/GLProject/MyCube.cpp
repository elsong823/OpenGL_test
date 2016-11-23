
#include "Utility.h"
#include "Shader.h"
#include "Camera.h"
#include "MyCube.h"

CMyCube::CMyCube()
{
}

CMyCube::CMyCube(GLchar * _vertexShaderPath, GLchar * _fragmentShaderPath, CCamera * _camera)
{
	m_camera = _camera;
	Init(_vertexShaderPath, _fragmentShaderPath);
}

CMyCube::~CMyCube()
{

}

void CMyCube::Init(GLchar * _vertexShaderPath, GLchar * _fragmentShaderPath)
{
	m_matView = m_camera->GetViewMat();
	m_matProjection = m_camera->GetProjectionMat();
	m_shader = new CShader(_vertexShaderPath, _fragmentShaderPath);
	m_program = m_shader->GetProgram();
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenTextures(1, &m_texture1);
	glGenTextures(1, &m_texture2);

	BindArray();
	m_rotateSpeed = 1.20f;
	//m_matModel = glm::translate(m_matModel, glm::vec3(5.2f, 3.0f, -5.0f));
}

void CMyCube::BindArray()
{
	GLfloat _vertices[] = {
		//1
		0.5, -0.5, 0.5, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		-0.5, 0.5, 0.5, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		-0.5, -0.5, 0.5, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

		0.5, -0.5, 0.5, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.5, 0.5, 0.5, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		-0.5, 0.5, 0.5, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		//2
		0.5, -0.5, -0.5, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.5, 0.5, 0.5, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.5, -0.5, 0.5, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

		0.5, -0.5, -0.5, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.5, 0.5, -0.5, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.5, 0.5, 0.5, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		//3
		-0.5, -0.5, -0.5, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		-0.5, 0.5, -0.5, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.5, -0.5, -0.5, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

		-0.5, 0.5, -0.5, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.5, 0.5, -0.5, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.5, -0.5, -0.5, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		//4
		-0.5, -0.5, 0.5, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		-0.5, 0.5, 0.5, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		-0.5, -0.5, -0.5, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

		-0.5, 0.5, 0.5, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		-0.5, 0.5, -0.5, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		-0.5, -0.5, -0.5, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		//5
		0.5, 0.5, 0.5, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.5, 0.5, -0.5, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		-0.5, 0.5, -0.5, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,

		0.5, 0.5, 0.5, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		-0.5, 0.5, -0.5, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		-0.5, 0.5, 0.5, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		//6
		-0.5, -0.5, 0.5, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		-0.5, -0.5, -0.5, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.5, -0.5, 0.5, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

		-0.5, -0.5, -0.5, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.5, -0.5, -0.5, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.5, -0.5, 0.5, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
	};
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(_vertices), _vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 8, (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 8, (GLvoid*)(sizeof(GLfloat)* 3));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 8, (GLvoid*)(sizeof(GLfloat)* 6));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, m_texture1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	int _imageWidth, _imageHeight;
	auto _texSource = LoadImage("../bin/res/container.jpg", _imageWidth, _imageHeight);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _imageWidth, _imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, _texSource);
	SOIL_free_image_data(_texSource);
	_texSource = nullptr;
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, m_texture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	_texSource = LoadImage("../bin/res/awesomeface.png", _imageWidth, _imageHeight);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _imageWidth, _imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, _texSource);
	SOIL_free_image_data(_texSource);
	_texSource = nullptr;
	glBindTexture(GL_TEXTURE_2D, 0);
}

void CMyCube::Draw(float _deltaTime)
{
	if (!m_matView || !m_matProjection)
		return;

	if (m_vecRotateAxis.x != 0
		|| m_vecRotateAxis.y != 0
		|| m_vecRotateAxis.z != 0)
	{
		m_matModel = glm::rotate(m_matModel, _deltaTime * m_rotateSpeed, m_vecRotateAxis);
	}

	glUseProgram(m_program);
	auto _modelTarget = glGetUniformLocation(m_program, "model");
	auto _viewTarget = glGetUniformLocation(m_program, "view");
	auto _projectionTarget = glGetUniformLocation(m_program, "projection");
	glUniformMatrix4fv(_modelTarget, 1, GL_FALSE, glm::value_ptr(m_matModel));
	glUniformMatrix4fv(_viewTarget, 1, GL_FALSE, glm::value_ptr(*m_matView));
	glUniformMatrix4fv(_projectionTarget, 1, GL_FALSE, glm::value_ptr(*m_matProjection));

	//Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture1);
	glUniform1i(glGetUniformLocation(m_program, "tex1"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_texture2);
	glUniform1i(glGetUniformLocation(m_program, "tex2"), 1);

	glBindVertexArray(m_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

GLubyte * CMyCube::LoadImage(GLchar * _path, int & _imageWidth, int & _imageHeight)
{
	GLubyte * _source = nullptr;
	_source = SOIL_load_image(_path, &_imageWidth, &_imageHeight, nullptr, SOIL_LOAD_RGBA);
	return _source;
}

void CMyCube::SetPosition(float _x, float _y, float _z)
{
	m_matModel = glm::translate(m_matModel, glm::vec3(_x, _y, _z));
}

void CMyCube::SetRotateAxis(float _x, float _y, float _z)
{
	m_vecRotateAxis = glm::vec3(_x, _y, _z);
	m_vecRotateAxis = glm::normalize(m_vecRotateAxis);
}
