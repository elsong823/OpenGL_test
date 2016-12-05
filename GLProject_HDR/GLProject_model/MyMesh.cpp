#include "Utility.h"
#include "MyMesh.h"

CMyMesh::CMyMesh()
{
}

CMyMesh::~CMyMesh()
{
}

CMyMesh::CMyMesh(vector<VERTEX> _vertices, vector<GLuint> _indices, vector<TEXTURE> _textures)
:m_VAO(0),
m_VBO(0),
m_EBO(0)
{
	m_vertices = _vertices;
	m_indices = _indices;
	m_textures = _textures;
	this->SetupMesh();
}

void CMyMesh::Draw(GLuint _shaderProgram)
{
	//BindTexture
	string type;
	string name;
	int numDiff = 1;
	int numSpec = 1;
	for (int i = 0; i < m_textures.size(); ++i)
	{
		type = m_textures[i].type;
		if (type == "tex_diffuse")
			name = to_string(numDiff++);
		else if(type == "tex_specular")
			name = to_string(numSpec++);
		type = type + name;
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
		glUniform1i(glGetUniformLocation(_shaderProgram, type.c_str()), i);
	}

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VERTEX), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VERTEX), (GLvoid*)(offsetof(VERTEX, normal)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VERTEX), (GLvoid*)(offsetof(VERTEX, texcoords)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), &m_indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}
