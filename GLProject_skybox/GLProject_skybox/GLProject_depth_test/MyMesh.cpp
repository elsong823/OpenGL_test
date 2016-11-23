#include "Utility.h"	
#include "MyMesh.h"

CMyMesh::CMyMesh()
{

}

CMyMesh::~CMyMesh()
{

}

void CMyMesh::SetupMesh()
{
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);
	
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(VERTEX), &m_vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VERTEX), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VERTEX), (GLvoid*)(offsetof(VERTEX, normal)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VERTEX), (GLvoid*)(offsetof(VERTEX, texcoords)));
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), &m_indices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void CMyMesh::Draw(GLuint _shaderProgram)
{
	//≈–∂œŒ∆¿Ì¿‡–Õ
	int texDiffuseNum = 0;
	int texSpecularNum = 0;
	GLchar texName[128] = {};
	for (int i = 0; i < m_textures.size(); i++)
	{
		memset(texName, 0, sizeof(texName));
		if (m_textures[i].type == "diffuse")
		{
			sprintf_s(texName, sizeof(texName), "texDiffuse_%d", ++texDiffuseNum);
		}
		else if (m_textures[i].type == "specular")
		{
			sprintf_s(texName, sizeof(texName), "texSpecular_%d", ++texSpecularNum);
		}
		else if (m_textures[i].type == "reflect")
		{
			sprintf_s(texName, sizeof(texName), "texReflect_%d", ++texSpecularNum);
		}

		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
		glUniform1i(glGetUniformLocation(_shaderProgram, texName), i);
	}
	if (m_skyBox > 0)
	{
		glActiveTexture(GL_TEXTURE0 + m_textures.size());
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyBox);
		glUniform1i(glGetUniformLocation(_shaderProgram, "texSkyCube"), m_textures.size());
	}
	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glBindVertexArray(0);
}
