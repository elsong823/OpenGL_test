#include "Utility.h"
#include "PointLight.h"
#include "MyShaderLoader.h"
#include "MyCamera.h"
#include "MyCube.h"

GLuint CMyCube::VBO = 0;
GLuint CMyCube::VAO = 0;
GLuint CMyCube::texDiffuse = 0;
GLuint CMyCube::texEmission = 0;
GLuint CMyCube::texSpecular = 0;
glm::vec3 CMyCube::vecLightDir = glm::vec3(0, 0, 0);
vector<CPointLight*> * CMyCube::m_pointLights = nullptr;


void CMyCube::SetLightDir(float _x, float _y, float _z)
{
	vecLightDir.x = _x;
	vecLightDir.y = _y;
	vecLightDir.z = _z;
}

CMyCube::CMyCube()
{
}

CMyCube::~CMyCube()
{
}

CMyCube::CMyCube(CMyCamera * _camera)
:m_shaderProgram(0),
m_dirty(true),
m_rotationAxis(0.0001f, 0.0f, 0.0f),
m_scale(1.0f, 1.0f, 1.0f),
m_rotateDegree(0.0f),
m_matView(nullptr),
m_matProjection(nullptr),
m_shininess(32),
m_camera(_camera),
m_timer(0.0f)
{
	this->InitVertex();
	this->InitTexture();
	m_matView = _camera->GetView();
	m_matProjection = _camera->GetProjection();
	//Set material and light
	m_material.ambient = glm::vec3(1.0f, 0.5f, 0.31f);
	m_material.diffuse = glm::vec3(1.0f, 0.5f, 0.31f);
	m_material.specular = glm::vec3(0.5f, 0.5f, 0.5f);
	m_material.shininess = 32.0f;
	m_cutoff = cos(glm::radians(12.5f));
	m_outCutoff = cos(glm::radians(20.0f));
	m_lighting.ambient = glm::vec3(0.1f);
	m_lighting.diffuse = glm::vec3(0.2f);
	m_lighting.specular = glm::vec3(1.0f);
	m_directLight.direct = glm::normalize(glm::vec3(-0.5, -0.2, 0.0f));
	m_directLight.ambient = glm::vec3(0.1f);
	m_directLight.diffuse = glm::vec3(0.3f);
	m_directLight.specular = glm::vec3(1.0f);
}

void CMyCube::SetShader(GLchar * _verShaderPath, GLchar * _fragShader)
{
	if (m_shaderProgram)
	{
		CMyShaderLoader::GetInstance()->DeleteProgram(m_shaderProgram);
		m_shaderProgram = 0;
	}
	m_shaderProgram = CMyShaderLoader::GetInstance()->CreateProgram(_verShaderPath, _fragShader);
}

void CMyCube::Update(GLfloat _dt)
{
	m_timer += _dt * 0.2;
	glUseProgram(m_shaderProgram);
	glUniform1f(glGetUniformLocation(m_shaderProgram, "fCoordAdd"), m_timer);

	//test
	if (m_dirty)
	{
		m_matModel = glm::mat4();
		m_matModel = glm::translate(m_matModel, m_position);
		m_matModel = glm::rotate(m_matModel, glm::radians(m_rotateDegree), m_rotationAxis);
		m_matModel = glm::scale(m_matModel, m_scale);
		m_dirty = false;
	}
}

void CMyCube::Draw()
{
	glUseProgram(m_shaderProgram);
	
	//Set Matrix
	glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(m_matModel));
	glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "matView"), 1, GL_FALSE, glm::value_ptr(*m_matView));
	glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "matProjection"), 1, GL_FALSE, glm::value_ptr(*m_matProjection));

	//SetCamera
	glUniform3fv(glGetUniformLocation(m_shaderProgram, "fragViewPos"), 1, glm::value_ptr(m_camera->GetPosition()));

	//Set Light
	glUniform3fv(glGetUniformLocation(m_shaderProgram, "directLight.direct"), 1, glm::value_ptr(m_directLight.direct));
	glUniform3fv(glGetUniformLocation(m_shaderProgram, "directLight.ambient"), 1, glm::value_ptr(m_directLight.ambient));
	glUniform3fv(glGetUniformLocation(m_shaderProgram, "directLight.diffuse"), 1, glm::value_ptr(m_directLight.diffuse));
	glUniform3fv(glGetUniformLocation(m_shaderProgram, "directLight.specular"), 1, glm::value_ptr(m_directLight.specular));

	//Set Point light
	int _i = 0;
	char _targetName[32] = {};
	for (auto light : *m_pointLights)
	{
		sprintf_s(_targetName, sizeof(_targetName), "pointLights[%d].position", _i);
		glUniform3fv(glGetUniformLocation(m_shaderProgram, _targetName), 1, glm::value_ptr(*light->GetPosition()));
		sprintf_s(_targetName, sizeof(_targetName), "pointLights[%d].ambient", _i);
		glUniform3fv(glGetUniformLocation(m_shaderProgram, _targetName), 1, glm::value_ptr(*light->GetLightColor()));
		sprintf_s(_targetName, sizeof(_targetName), "pointLights[%d].diffuse", _i);
		glUniform3fv(glGetUniformLocation(m_shaderProgram, _targetName), 1, glm::value_ptr(*light->GetLightColor()));
		sprintf_s(_targetName, sizeof(_targetName), "pointLights[%d].specular", _i);
		glUniform3fv(glGetUniformLocation(m_shaderProgram, _targetName), 1, glm::value_ptr(*light->GetLightColor()));
		sprintf_s(_targetName, sizeof(_targetName), "pointLights[%d].constant", _i);
		glUniform1f(glGetUniformLocation(m_shaderProgram, _targetName), 1.0f);
		sprintf_s(_targetName, sizeof(_targetName), "pointLights[%d].linear", _i);
		glUniform1f(glGetUniformLocation(m_shaderProgram, _targetName), light->GetLinear());
		sprintf_s(_targetName, sizeof(_targetName), "pointLights[%d].quadratic", _i);
		glUniform1f(glGetUniformLocation(m_shaderProgram, _targetName), light->GetQuadratic());
		_i++;
	}

	//Set Spot light
	glUniform3fv(glGetUniformLocation(m_shaderProgram, "spotLight.position"), 1, glm::value_ptr(m_camera->GetPosition()));
	glUniform3fv(glGetUniformLocation(m_shaderProgram, "spotLight.lightDir"), 1, glm::value_ptr(m_camera->GetFaceTo()));
	glUniform1f(glGetUniformLocation(m_shaderProgram, "spotLight.cutoff"), m_cutoff);
	glUniform1f(glGetUniformLocation(m_shaderProgram, "spotLight.outCutoff"), m_outCutoff);
	glUniform3f(glGetUniformLocation(m_shaderProgram, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
	glUniform3f(glGetUniformLocation(m_shaderProgram, "spotLight.diffuse"), 0.5f, 0.5f, 0.5f);
	glUniform3f(glGetUniformLocation(m_shaderProgram, "spotLight.specular"), 1.0f, 1.0f, 1.0f);
	glUniform1f(glGetUniformLocation(m_shaderProgram, "spotLight.constant"), 1.0f);
	glUniform1f(glGetUniformLocation(m_shaderProgram, "spotLight.linear"), 0.35f);
	glUniform1f(glGetUniformLocation(m_shaderProgram, "spotLight.quadratic"), 0.44f);

	//Set material
	glUniform1i(glGetUniformLocation(m_shaderProgram, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(m_shaderProgram, "material.specular"), 1);
	glUniform1f(glGetUniformLocation(m_shaderProgram, "material.shininess"), 32.0f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, CMyCube::texDiffuse);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, CMyCube::texSpecular);
	//glActiveTexture(GL_TEXTURE2);
	//glBindTexture(GL_TEXTURE_2D, CMyCube::texEmission);
	glBindVertexArray(CMyCube::VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void CMyCube::InitVertex()
{
	if (CMyCube::VBO)
	{
		return;
	}
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 8, (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 8, (GLvoid*)(sizeof(GLfloat)* 3));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 8, (GLvoid*)(sizeof(GLfloat)* 6));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void CMyCube::InitTexture()
{
	if (!CMyCube::texDiffuse)
	{
		glGenTextures(1, &CMyCube::texDiffuse);
		glGenTextures(1, &CMyCube::texSpecular);
		glGenTextures(1, &CMyCube::texEmission);

		GLubyte * _imageSource = nullptr;
		GLint _texWidth = 0;
		GLint _texHeight = 0;
		//Tex Diffuse
		_imageSource = SOIL_load_image("../bin/res/container2.png", &_texWidth, &_texHeight, nullptr, SOIL_LOAD_RGB);
		glBindTexture(GL_TEXTURE_2D, CMyCube::texDiffuse);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _texWidth, _texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, _imageSource);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		//Tex Specular
		_imageSource = SOIL_load_image("../bin/res/container2_specular.png", &_texWidth, &_texHeight, nullptr, SOIL_LOAD_RGB);
		glBindTexture(GL_TEXTURE_2D, CMyCube::texSpecular);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _texWidth, _texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, _imageSource);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		//Tex emission
		_imageSource = SOIL_load_image("../bin/res/matrix.jpg", &_texWidth, &_texHeight, nullptr, SOIL_LOAD_RGB);
		glBindTexture(GL_TEXTURE_2D, CMyCube::texEmission);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _texWidth, _texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, _imageSource);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void CMyCube::SetPosition(glm::vec3 _pos)
{
	m_position = _pos;
	m_dirty = true;
}

void CMyCube::SetRotation(glm::vec3 _axis, GLfloat _degree)
{
	m_rotationAxis = _axis;
	m_rotateDegree = _degree;
	m_dirty = true;
}

void CMyCube::SetScale(glm::vec3 _scale)
{
	m_scale = _scale;
	m_dirty = true;
}

