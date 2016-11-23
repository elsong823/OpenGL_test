#pragma once

#include "MyMesh.h"

class CMyCamera;

class CMyModel
{
public:
	CMyModel(const char * _path, GLuint _skyBoxID);
	~CMyModel();

public:
	void Draw(GLuint _shaderProgram, CMyCamera * _camera, int _drawType);
	void SetPos(glm::vec3 _pos){ m_position = _pos; }
	void SetScale(glm::vec3 _scale){ m_scale = _scale; }

private:
	CMyModel();
	void ProcessNode(aiNode * _node, const aiScene * _scene);
	void ProcessMesh(aiMesh * _mesh, const aiScene * _scene);

private:
	string m_directory;
	vector<CMyMesh*> m_meshs;

	glm::vec3 m_position;
	glm::vec3 m_scale;
};

