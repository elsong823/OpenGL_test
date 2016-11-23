#pragma once

#define POINT_LIGHT_CONST 1.0f

class CMyCamera;

class CPointLight
{
public:
	CPointLight(CMyCamera * _camera, glm::vec3 _pos, float _linear, float _quadratic, glm::vec3 _lightColor);
	~CPointLight();

private:
	CPointLight();

public:
	const glm::vec3 * GetPosition(){ return &m_position; }
	const glm::vec3 * GetLightColor(){ return &m_lightColor; }
	void SetLightColor(float _x, float _y, float _z){ m_lightColor.r = _x; m_lightColor.g = _y; m_lightColor.b = _z; }
	float GetLinear(){ return m_linear; }
	float GetQuadratic(){ return m_quadratic; }

	void Draw();

private:
	void Init();

private:
	static GLuint VAO;
	static GLuint VBO;

	glm::vec3 m_position;
	glm::vec3 m_lightColor;
	CMyCamera * m_camera;
	const glm::mat4 * m_matView;
	const glm::mat4 * m_matProjection;
	glm::mat4 m_matModel;

	float m_linear;
	float m_quadratic;
	float m_const;

	GLuint m_shaderProgram;
};

