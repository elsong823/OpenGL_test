
#pragma once

class CMyCamera;
class CMyMesh;

class CMyModel
{
public:
	CMyModel(GLchar * _path);
	~CMyModel();

public:
	void SetCamera(CMyCamera * _camera);
	void SetPosition(glm::vec3 _position){ m_position = _position; m_dirty = true; }
	void SetScale(glm::vec3 _scale){ m_scale = _scale; m_dirty = true; }
	void SetRotation(glm::vec3 _axis, GLfloat _degree){ m_rotateAxis = _axis; m_degree = _degree; m_dirty = true; }
	void Update(GLfloat _dt);
	void Draw(GLuint _shaderProgram);
	static void SetDirectLight(D_LIGHT _dLight){ directLight = _dLight; }
	void AddPointLight(PT_LIGHT _ptLight);
	void SetSpotLight(S_LIGHT _sLight){ m_spotLight = _sLight; }
	void SetLightDir(glm::vec3 lightDir){ m_lightDir = lightDir; }

private:
	CMyModel();
	void ProcessNode(const aiScene * _scene, aiNode * _node);
	CMyMesh * ProcessMesh(const aiScene * _scene, aiMesh * _mesh);
	void ProcessMaterial(const aiScene * _scene, GLint _materialIdx, vector<TEXTURE> & _textures);
	vector<TEXTURE> LoadMaterialTextures(aiMaterial * _material, aiTextureType _type, string _typeName);
	GLuint GetLocalTexture(const GLchar * _texName);

private:
	static D_LIGHT directLight;
	static map<string, GLuint> m_textures;
	CMyCamera * m_camera;
	bool m_dirty;
	glm::vec3 m_position;
	glm::vec3 m_scale;
	glm::vec3 m_rotateAxis;
	GLfloat m_degree;
	glm::mat4 m_matModel;
	const glm::mat4 * m_matView;
	const glm::mat4 * m_matProjection;
	string m_directory;
	vector<CMyMesh*> m_meshes;
	vector<PT_LIGHT> m_ptLights;
	S_LIGHT m_spotLight;
	glm::vec3 m_lightDir;
};

