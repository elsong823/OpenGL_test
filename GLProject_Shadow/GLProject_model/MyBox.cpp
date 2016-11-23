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
	//´«µÝ²ÄÖÊ
	glUniform1i(glGetUniformLocation(m_shaderProgram, "material.texDiffuse"), 0);
	glUniform1i(glGetUniformLocation(m_shaderProgram, "material.texSpecular"), 1);
	glUniform1f(glGetUniformLocation(m_shaderProgram, "material.shininess"), material.shininess);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, material.texDiffuse);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, material.texSpecular);
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