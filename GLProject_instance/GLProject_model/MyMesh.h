
class CMyMesh
{
public:
	CMyMesh(vector<VERTEX> _vertices, vector<GLuint> _indices, vector<TEXTURE> _textures);
	~CMyMesh();
private:
	CMyMesh();
	void SetupMesh();

public:
	void Draw(GLuint _shaderProgram);
	void SetupInstanceData(glm::mat4 * _modelMatrices, GLint _count);

private:
	GLuint m_VAO;
	GLuint m_VBO;
	GLuint m_EBO;
	vector<VERTEX> m_vertices;
	vector<GLuint> m_indices;
	vector<TEXTURE> m_textures;

	GLuint m_instanceVBO;
	GLint m_instanceCount;
};
