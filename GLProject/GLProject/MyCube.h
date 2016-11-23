
class CShader;
class CCamera;

class CMyCube
{
public:
	CMyCube(GLchar * _vertexShaderPath, GLchar * _fragmentShaderPath, CCamera * _camera);
	~CMyCube();
private:
	CMyCube();

public:
	void Init(GLchar * _vertexShaderPath, GLchar * _fragmentShaderPath);
	void Draw(float _deltaTime);
	void SetPosition(float _x, float _y, float _z);
	void SetRotateAxis(float _x, float _y, float _z);

private:
	void BindArray();
	GLubyte * LoadImage(GLchar * _path, int & _imageWidth, int & _imageHeight);

private:
	CCamera * m_camera;
	const glm::mat4 * m_matView;
	const glm::mat4 * m_matProjection;
	CShader * m_shader;
	GLuint m_program;

	glm::mat4 m_matModel;
	GLuint m_VAO;
	GLuint m_VBO;
	GLuint m_texture1;
	GLuint m_texture2;

	GLfloat m_rotateSpeed;
	glm::vec3 m_vecRotateAxis;

};