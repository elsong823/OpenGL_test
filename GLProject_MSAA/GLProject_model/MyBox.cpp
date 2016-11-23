#include "Utility.h"
#include "MyCamera.h"
#include "MyBox.h"

D_LIGHT CMyBox::directLight;
MATERIAL CMyBox::material;
S_LIGHT CMyBox::spotLight;

CMyBox::CMyBox()
{
	this->Init();
}

CMyBox::~CMyBox()
{
}

void CMyBox::SubsetDraw()
{
	//传递位置
	glUniform3fv(glGetUniformLocation(m_shaderProgram, "viewPos"), 1, glm::value_ptr(*cameraPos));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, CMyBox::material.texDiffuse);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, CMyBox::material.texSpecular);
	//传递材质
	glUniform1i(glGetUniformLocation(m_shaderProgram, "material.texDiffuse"), 0);
	glUniform1i(glGetUniformLocation(m_shaderProgram, "material.texSpecular"), 1);
	glUniform1f(glGetUniformLocation(m_shaderProgram, "material.shininess"), material.shininess);
	//传递光照
	glUniform3fv(glGetUniformLocation(m_shaderProgram, "directLight.direction"), 1, glm::value_ptr(directLight.direction));
	glUniform3fv(glGetUniformLocation(m_shaderProgram, "directLight.ambient"), 1, glm::value_ptr(directLight.ambient));
	glUniform3fv(glGetUniformLocation(m_shaderProgram, "directLight.diffuse"), 1, glm::value_ptr(directLight.diffuse));
	glUniform3fv(glGetUniformLocation(m_shaderProgram, "directLight.specular"), 1, glm::value_ptr(directLight.specular));

	//传递点光源
	int idx = 0;
	char tempName[64] = {};
	for (auto ptLight : m_ptLights)
	{
		sprintf_s(tempName, sizeof(tempName), "pointLights[%d].position", idx);
		glUniform3fv(glGetUniformLocation(m_shaderProgram, tempName), 1, glm::value_ptr(ptLight.position));
		sprintf_s(tempName, sizeof(tempName), "pointLights[%d].ambient", idx);
		glUniform3fv(glGetUniformLocation(m_shaderProgram, tempName), 1, glm::value_ptr(ptLight.ambient));
		sprintf_s(tempName, sizeof(tempName), "pointLights[%d].diffuse", idx);
		glUniform3fv(glGetUniformLocation(m_shaderProgram, tempName), 1, glm::value_ptr(ptLight.diffuse));
		sprintf_s(tempName, sizeof(tempName), "pointLights[%d].specular", idx);
		glUniform3fv(glGetUniformLocation(m_shaderProgram, tempName), 1, glm::value_ptr(ptLight.specular));
		sprintf_s(tempName, sizeof(tempName), "pointLights[%d].constant", idx);
		glUniform1f(glGetUniformLocation(m_shaderProgram, tempName), ptLight.constant);
		sprintf_s(tempName, sizeof(tempName), "pointLights[%d].linear", idx);
		glUniform1f(glGetUniformLocation(m_shaderProgram, tempName), ptLight.linear);
		sprintf_s(tempName, sizeof(tempName), "pointLights[%d].quadratic", idx);
		glUniform1f(glGetUniformLocation(m_shaderProgram, tempName), ptLight.quadratic);
		++idx;
	}

	//传递聚光灯
	glUniform3fv(glGetUniformLocation(m_shaderProgram, "spotLight.position"), 1, glm::value_ptr(*cameraPos));
	glUniform3fv(glGetUniformLocation(m_shaderProgram, "spotLight.faceTo"), 1, glm::value_ptr(*m_camera->GetFaceTo()));
	glUniform3fv(glGetUniformLocation(m_shaderProgram, "spotLight.ambient"), 1, glm::value_ptr(spotLight.ambient));
	glUniform3fv(glGetUniformLocation(m_shaderProgram, "spotLight.diffuse"), 1, glm::value_ptr(spotLight.diffuse));
	glUniform3fv(glGetUniformLocation(m_shaderProgram, "spotLight.specular"), 1, glm::value_ptr(spotLight.specular));
	glUniform1f(glGetUniformLocation(m_shaderProgram, "spotLight.cutoff"), spotLight.cutoff);
	glUniform1f(glGetUniformLocation(m_shaderProgram, "spotLight.outCutoff"), spotLight.outCutoff);
	glUniform1f(glGetUniformLocation(m_shaderProgram, "spotLight.constant"), spotLight.constant);
	glUniform1f(glGetUniformLocation(m_shaderProgram, "spotLight.quadratic"), spotLight.quadratic);
}

void CMyBox::Init()
{
	CMyCube::Init();
	if (!material.texDiffuse)
	{
		material.shininess = 128;

		glGenTextures(1, &material.texDiffuse);
		glGenTextures(1, &material.texSpecular);
		glBindTexture(GL_TEXTURE_2D, material.texDiffuse);
		GLubyte * texSource = nullptr;
		GLint imageWidth, imageHeight;

		texSource = SOIL_load_image("../bin/res/container2.png", &imageWidth, &imageHeight, nullptr, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, texSource);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		SOIL_free_image_data(texSource);

		glBindTexture(GL_TEXTURE_2D, material.texSpecular);
		texSource = SOIL_load_image("../bin/res/container2_specular.png", &imageWidth, &imageHeight, nullptr, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, texSource);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		SOIL_free_image_data(texSource);
		texSource = nullptr;
	}
}

void CMyBox::AddPointLight(PT_LIGHT _light)
{
	m_ptLights.emplace_back(_light);
}