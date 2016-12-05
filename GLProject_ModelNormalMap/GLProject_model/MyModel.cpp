#include "Utility.h"
#include "MyCamera.h"
#include "MyMesh.h"
#include "MyModel.h"

map<string, GLuint> CMyModel::m_textures;
D_LIGHT CMyModel::directLight;

CMyModel::CMyModel()
{
}

CMyModel::~CMyModel()
{
}

CMyModel::CMyModel(GLchar * _path)
:m_dirty(true),
m_matView(nullptr),
m_matProjection(nullptr),
m_scale(1.0f),
m_position(0.0f)
{
	Assimp::Importer importer;
	const aiScene * scene = importer.ReadFile(_path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		printf_s("Read model failed\n");
		return;
	}
	m_directory = _path;
	m_directory = m_directory.substr(0, m_directory.find_last_of('/'));
	this->ProcessNode(scene, scene->mRootNode);
	int a = 1;
}

void CMyModel::SetCamera(CMyCamera * _camera)
{
	m_camera = _camera;
	m_matView = _camera->GetView();
	m_matProjection = _camera->GetProjection();
	m_dirty = true;
}

void CMyModel::Update(GLfloat _dt)
{
	if (m_dirty)
	{
		m_matModel = glm::mat4();
		m_matModel = glm::translate(m_matModel, m_position);
		m_matModel = glm::rotate(m_matModel, glm::radians(m_degree), m_rotateAxis);
		m_matModel = glm::scale(m_matModel, m_scale);
		m_dirty = false;
	}
}

void CMyModel::Draw(GLuint _shaderProgram)
{
	glUseProgram(_shaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(_shaderProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(m_matModel));
	glUniformMatrix4fv(glGetUniformLocation(_shaderProgram, "matView"), 1, GL_FALSE, glm::value_ptr(*m_matView));
	glUniformMatrix4fv(glGetUniformLocation(_shaderProgram, "matProjection"), 1, GL_FALSE, glm::value_ptr(*m_matProjection));

	glUniform3fv(glGetUniformLocation(_shaderProgram, "viewPos"), 1, glm::value_ptr(*m_camera->GetPosition()));
	glUniform3fv(glGetUniformLocation(_shaderProgram, "lightDir"), 1, glm::value_ptr(m_lightDir));
	//d light
	//glUniform3fv(glGetUniformLocation(_shaderProgram, "dLight.lightDir"), 1, glm::value_ptr(directLight.direction));
	//glUniform3fv(glGetUniformLocation(_shaderProgram, "dLight.ambient"), 1, glm::value_ptr(directLight.ambient));
	//glUniform3fv(glGetUniformLocation(_shaderProgram, "dLight.diffuse"), 1, glm::value_ptr(directLight.diffuse));
	//glUniform3fv(glGetUniformLocation(_shaderProgram, "dLight.specular"), 1, glm::value_ptr(directLight.specular));
	//glUniform1f(glGetUniformLocation(_shaderProgram, "shiniess"), 128);
	//pt light
	//char name[128] = {};
	//for (int i = 0; i < m_ptLights.size(); ++i)
	//{
	//	//sprintf_s(name, sizeof(name), "ptLights[%d].lightPosition", i);
	//	//glUniform3fv(glGetUniformLocation(_shaderProgram, name), 1, glm::value_ptr(m_ptLights[i].position));
	//	//sprintf_s(name, sizeof(name), "ptLights[%d].ambient", i);
	//	//glUniform3fv(glGetUniformLocation(_shaderProgram, name), 1, glm::value_ptr(m_ptLights[i].ambient));
	//	//sprintf_s(name, sizeof(name), "ptLights[%d].diffuse", i);
	//	//glUniform3fv(glGetUniformLocation(_shaderProgram, name), 1, glm::value_ptr(m_ptLights[i].diffuse));
	//	//sprintf_s(name, sizeof(name), "ptLights[%d].specular", i);
	//	//glUniform3fv(glGetUniformLocation(_shaderProgram, name), 1, glm::value_ptr(m_ptLights[i].specular));
	//	//sprintf_s(name, sizeof(name), "ptLights[%d].constant", i);
	//	//glUniform1f(glGetUniformLocation(_shaderProgram, name), m_ptLights[i].constant);
	//	//sprintf_s(name, sizeof(name), "ptLights[%d].linear", i);
	//	//glUniform1f(glGetUniformLocation(_shaderProgram, name), m_ptLights[i].linear);
	//	//sprintf_s(name, sizeof(name), "ptLights[%d].quadratic", i);
	//	//glUniform1f(glGetUniformLocation(_shaderProgram, name), m_ptLights[i].quadratic);
	//}
	////Spot light
	////glUniform3fv(glGetUniformLocation(_shaderProgram, "spotLight.position") ,1, glm::value_ptr(*m_camera->GetPosition()));
	////glUniform3fv(glGetUniformLocation(_shaderProgram, "spotLight.faceTo"), 1, glm::value_ptr(*m_camera->GetFaceTo()));
	////glUniform3fv(glGetUniformLocation(_shaderProgram, "spotLight.ambient"), 1, glm::value_ptr(m_spotLight.ambient));
	////glUniform3fv(glGetUniformLocation(_shaderProgram, "spotLight.diffuse"), 1, glm::value_ptr(m_spotLight.diffuse));
	////glUniform3fv(glGetUniformLocation(_shaderProgram, "spotLight.specular"), 1, glm::value_ptr(m_spotLight.specular));
	////glUniform1f(glGetUniformLocation(_shaderProgram, "spotLight.constant"), m_spotLight.constant);
	////glUniform1f(glGetUniformLocation(_shaderProgram, "spotLight.linear"), m_spotLight.linear);
	////glUniform1f(glGetUniformLocation(_shaderProgram, "spotLight.quadratic"), m_spotLight.quadratic);
	////glUniform1f(glGetUniformLocation(_shaderProgram, "spotLight.cutoff"), m_spotLight.cutoff);
	////glUniform1f(glGetUniformLocation(_shaderProgram, "spotLight.outCutoff"), m_spotLight.outCutoff);

	for (auto mesh : m_meshes)
	{
		mesh->Draw(_shaderProgram);
	}
}

void CMyModel::ProcessNode(const aiScene * _scene, aiNode * _node)
{
	//处理自己的网格
	for (int i = 0; i < _node->mNumMeshes; ++i)
	{
		auto mesh = this->ProcessMesh(_scene, _scene->mMeshes[_node->mMeshes[i]]);
		m_meshes.push_back(mesh);
	}
	//遍历子节点
	for (int i = 0; i < _node->mNumChildren; ++i)
	{
		this->ProcessNode(_scene, _node->mChildren[i]);
	}
}

CMyMesh * CMyModel::ProcessMesh(const aiScene * _scene, aiMesh * _mesh)
{
	vector<VERTEX> vertices;
	vector<GLuint> indices;
	vector<TEXTURE> textures;
	for (int i = 0; i < _mesh->mNumVertices; ++i)
	{
		VERTEX vertex;
		vertex.position.x = _mesh->mVertices[i].x;
		vertex.position.y = _mesh->mVertices[i].y;
		vertex.position.z = _mesh->mVertices[i].z;
		vertex.normal2.x = _mesh->mNormals[i].x;
		vertex.normal2.y = _mesh->mNormals[i].y;
		vertex.normal2.z = _mesh->mNormals[i].z;
		//直接读取
		vertex.tangent.x = _mesh->mTangents[i].x;
		vertex.tangent.y = _mesh->mTangents[i].y;
		vertex.tangent.z = _mesh->mTangents[i].z;
		vertex.tangent = glm::normalize(vertex.tangent);
		vertex.bitangent.x = _mesh->mBitangents[i].x;
		vertex.bitangent.y = _mesh->mBitangents[i].y;
		vertex.bitangent.z = _mesh->mBitangents[i].z;
		vertex.bitangent = glm::normalize(vertex.bitangent);
		vertex.normal = glm::cross(vertex.tangent, vertex.bitangent);

		if (_mesh->mTextureCoords[0])
		{
			vertex.texcoords.x = _mesh->mTextureCoords[0][i].x;
			vertex.texcoords.y = _mesh->mTextureCoords[0][i].y;
		}
		vertices.push_back(vertex);
	}

	for (int i = 0; i < _mesh->mNumFaces; i++)
	{
		aiFace face = _mesh->mFaces[i];
		for (int  j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}
	if (_mesh->mMaterialIndex >= 0)
	{
		this->ProcessMaterial(_scene, _mesh->mMaterialIndex, textures);
	}

	CMyMesh * mesh = new CMyMesh(vertices, indices, textures);
	return mesh;
}

void CMyModel::ProcessMaterial(const aiScene * _scene, GLint _materialIdx, vector<TEXTURE> & _textures)
{
	if (_materialIdx < 0)
		return;

	aiMaterial * material = _scene->mMaterials[_materialIdx];
	vector<TEXTURE> texDiff = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "tex_diffuse");
	_textures.insert(_textures.end(), texDiff.begin(), texDiff.end());
	vector<TEXTURE> texSpec = LoadMaterialTextures(material, aiTextureType_SPECULAR, "tex_specular");
	_textures.insert(_textures.end(), texSpec.begin(), texSpec.end());
	vector<TEXTURE> texNormalMap = LoadMaterialTextures(material, aiTextureType_HEIGHT, "tex_normalMap");
	_textures.insert(_textures.end(), texNormalMap.begin(), texNormalMap.end());
	vector<TEXTURE> texReflect = LoadMaterialTextures(material, aiTextureType_AMBIENT, "tex_reflect");
	_textures.insert(_textures.end(), texReflect.begin(), texReflect.end());
}

vector<TEXTURE> CMyModel::LoadMaterialTextures(aiMaterial * _material, aiTextureType _type, string _typeName)
{
	vector<TEXTURE> vec;
	aiString name;
	for (int i = 0; i < _material->GetTextureCount(_type); i++)
	{
		if (_type == aiTextureType_REFLECTION)
		{
			int a = 1;
		}
		//获取纹理名称
		_material->GetTexture(_type, i, &name);
		TEXTURE tex;
		tex.type = _typeName;
		tex.path = m_directory + '/' + name.C_Str();
		tex.id = this->GetLocalTexture(tex.path.c_str());
		vec.push_back(tex);
	}
	return vec;
}

GLuint CMyModel::GetLocalTexture(const GLchar * _texName)
{
	//从map中找
	if (m_textures.count(_texName))
		return m_textures[_texName];

	//没有
	GLuint texture = 0;
	GLubyte * source = nullptr;
	int texWidth, texHeight;
	glGenTextures(1, &texture);
	source = SOIL_load_image(_texName, &texWidth, &texHeight, nullptr, SOIL_LOAD_RGB);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, source);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(source);
	source = nullptr;
	m_textures.emplace(make_pair(_texName, texture));
	return texture;
}

void CMyModel::AddPointLight(PT_LIGHT _ptLight)
{
	m_ptLights.push_back(_ptLight);
}