#pragma once

class CMyCamera;
class CPointLight;

struct stMaterial
{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	int shininess;
};

struct stLighting
{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float cutoff;
};

struct stDirectLight
{
	glm::vec3 direct;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

class CMyCube
{
	//Constructor
public:
	CMyCube(CMyCamera * _camera);
	~CMyCube();

private:
	CMyCube();

	//Func
public:
	void SetShader(GLchar * _verShaderPath, GLchar * _fragShader);
	void Update(GLfloat _dt);
	void Draw();
	void SetPosition(glm::vec3 _pos);
	glm::vec3 GetPosition(){ return m_position; }
	void SetRotation(glm::vec3 _axis, GLfloat _radian);
	void SetScale(glm::vec3 _scale);
	void AddShininess(){ m_shininess++; m_shininess = m_shininess > 256 ? 256 : m_shininess; }
	void ReduceShininess(){ m_shininess--; m_shininess = m_shininess < 0 ? 0 : m_shininess; }
	static void SetLightDir(float _x, float _y, float _z);
	void SetLight(CPointLight * _light){ m_pointLight = _light; };

private:
	void InitVertex();
	void InitTexture();

public:
	static GLuint VBO;
	static GLuint VAO;
	static GLuint texDiffuse;
	static GLuint texSpecular;
	static GLuint texEmission;
	static glm::vec3 vecLightDir;

	//Need update mat
	GLboolean m_dirty;
	CMyCamera * m_camera;
	GLuint m_shaderProgram;

	glm::vec3 m_position;
	glm::vec3 m_rotationAxis;
	GLfloat m_rotateDegree;
	glm::vec3 m_scale;
	glm::mat4 m_matTranslate;
	glm::mat4 m_matScale;
	glm::mat4 m_matRotate;
	glm::mat4 m_matNormal;

	glm::mat4 m_matModel;
	const glm::mat4 * m_matView;
	const glm::mat4 * m_matProjection;

	//Light
	GLint m_shininess;

	stMaterial m_material;
	stLighting m_lighting;

	CPointLight * m_pointLight;

	float m_timer;
	float m_cutoff;
	float m_outCutoff;
	
	//Light
	stDirectLight m_directLight;

	//PointLihts
	static vector<CPointLight*> * m_pointLights;

};

