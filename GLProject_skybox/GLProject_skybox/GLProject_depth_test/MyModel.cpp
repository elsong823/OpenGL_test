#include "Utility.h"
#include "MyCamera.h"
#include "MyModel.h"

using namespace Assimp;

CMyModel::CMyModel()
{
}

CMyModel::~CMyModel()
{
}

CMyModel::CMyModel(const char * _path, GLuint _skyBoxID)
: m_scale(1.0f)
, m_position(0.0f)
{
	Importer imp;
	const aiScene * scene = imp.ReadFile(_path, aiProcess_FlipUVs | aiProcess_Triangulate);
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		printf_s("Load model failed/n");
	}
	m_directory = _path;
	m_directory = m_directory.substr(0, m_directory.find_last_of('/') + 1);
	this->ProcessNode(scene->mRootNode, scene);

	for (auto mesh : m_meshs)
	{
		mesh->SetSkyBox(_skyBoxID);
	}
}

void CMyModel::ProcessNode(aiNode * _node,const aiScene * _scene)
{
	for (int i = 0; i < _node->mNumMeshes; ++i)
	{
		auto meshIdx = _node->mMeshes[i];
		auto mesh = _scene->mMeshes[meshIdx];
		this->ProcessMesh(mesh, _scene);
	}

	for (int i = 0; i < _node->mNumChildren; ++i)
	{
		this->ProcessNode(_node->mChildren[i], _scene);
	}
}

void CMyModel::ProcessMesh(aiMesh * _mesh, const aiScene * _scene)
{
	auto mesh = new CMyMesh();
	auto vertices = mesh->GetVertices();
	auto indices = mesh->GetIndices();
	auto textures = mesh->GetTextures();
	//¶¥µã
	for (int i = 0; i < _mesh->mNumVertices; ++i)
	{
		VERTEX vertex;
		vertex.position = glm::vec3(_mesh->mVertices[i].x, _mesh->mVertices[i].y, _mesh->mVertices[i].z);
		vertex.normal = glm::vec3(_mesh->mNormals[i].x, _mesh->mNormals[i].y, _mesh->mNormals[i].z);
		if (_mesh->mTextureCoords)
		{
			vertex.texcoords = glm::vec2(_mesh->mTextureCoords[0][i].x, _mesh->mTextureCoords[0][i].y);
		}
		vertices->push_back(vertex);
	}
	//Ë÷Òý
	for (int i = 0; i < _mesh->mNumFaces; ++i)
	{
		auto face = _mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; ++j)
		{
			indices->push_back(face.mIndices[j]);
		}
	}
	//ÎÆÀí
	if (_mesh->mMaterialIndex >= 0)
	{
		auto material = _scene->mMaterials[_mesh->mMaterialIndex];
		aiString path;
		TEXTURE tex;

		path = "";
		material->GetTexture(aiTextureType_DIFFUSE, 0, &path);
		if (path.length > 0)
		{
			tex.type = "diffuse";
			tex.id = LoadTexture((m_directory + path.C_Str()).c_str(), false, GL_REPEAT);
			tex.path = path.C_Str();
			textures->push_back(tex);
		}


		path = "";
		material->GetTexture(aiTextureType_SPECULAR, 0, &path);
		if (path.length > 0)
		{
			tex.type = "specular";
			tex.id = LoadTexture((m_directory + path.C_Str()).c_str(), false, GL_REPEAT);
			tex.path = path.C_Str();
			textures->push_back(tex);
		}

		path = "";
		material->GetTexture(aiTextureType_AMBIENT, 0, &path);
		if (path.length > 0)
		{
			tex.type = "reflect";
			tex.id = LoadTexture((m_directory + path.C_Str()).c_str(), false, GL_REPEAT);
			tex.path = path.C_Str();
			textures->push_back(tex);
		}
	}

	mesh->SetupMesh();
	m_meshs.push_back(mesh);
}

void CMyModel::Draw(GLuint _shaderProgram, CMyCamera * _camera, int _drawType)
{
	static glm::mat4 matModel;
	glUseProgram(_shaderProgram);
	matModel = glm::mat4();
	matModel = glm::translate(matModel, m_position);
	matModel = glm::scale(matModel, m_scale);
	glUniformMatrix4fv(glGetUniformLocation(_shaderProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(matModel));
	glUniformMatrix4fv(glGetUniformLocation(_shaderProgram, "matView"), 1, GL_FALSE, glm::value_ptr(*_camera->GetView()));
	glUniformMatrix4fv(glGetUniformLocation(_shaderProgram, "matProjection"), 1, GL_FALSE, glm::value_ptr(*_camera->GetProjection()));
	glUniform1i(glGetUniformLocation(_shaderProgram, "renderType"), _drawType);
	glUniform3fv(glGetUniformLocation(_shaderProgram, "eyePos"), 1, glm::value_ptr(*_camera->GetPosition()));

	for (auto mesh : m_meshs)
	{
		mesh->Draw(_shaderProgram);
	}
}