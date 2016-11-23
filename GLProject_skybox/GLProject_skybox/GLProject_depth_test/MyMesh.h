#pragma once

class CMyMesh
{
public:	
	CMyMesh();
	~CMyMesh();

public:
	vector<VERTEX> * GetVertices(){ return & m_vertices; }
	vector<GLuint> * GetIndices(){ return & m_indices; }
	vector<TEXTURE> * GetTextures(){ return & m_textures; }
	void SetupMesh();
	void SetSkyBox(GLuint _skyBox){ m_skyBox = _skyBox; }

public:
	void Draw(GLuint _shaderProgram);

private:
	GLuint m_VAO, m_VBO, m_EBO;
	vector<VERTEX> m_vertices;
	vector<GLuint> m_indices;
	vector<TEXTURE> m_textures;
	GLuint m_skyBox;
};

