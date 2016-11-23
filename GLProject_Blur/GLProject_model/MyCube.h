#pragma once

class CMyCamera;

class CMyCube
{
public:
	CMyCube();
	~CMyCube();

public:
	void Draw(GLuint _shaderProgram);
	void SetCamera(CMyCamera * _camera);
	void Update(GLfloat _dt);
	void SetPosition(glm::vec3 _pos);
	void SetRotation(glm::vec3 _axis, GLfloat _degree);
	void SetScale(glm::vec3 _scale);

	glm::vec3 GetPosition(){ return m_position; }

protected:
	virtual void Init();
	virtual void SubsetDraw(){}

protected:
	static GLuint VAO;
	static GLuint VBO;
	static const glm::vec3 * cameraPos;

	GLuint m_shaderProgram;

	CMyCamera * m_camera;
	const glm::mat4 * m_matView;
	const glm::mat4 * m_matProjection;

	bool m_dirty;
	glm::vec3 m_position;
	glm::vec3 m_rotateAxis;
	GLfloat m_radian;
	glm::vec3 m_scale;
	glm::mat4 m_matModel;
};

