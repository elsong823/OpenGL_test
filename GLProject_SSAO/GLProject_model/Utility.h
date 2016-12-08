#ifndef UTILITY_H
#define UTILITY_H
#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <SOIL/SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <time.h>
#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <random>
using namespace std;
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <ft2build.h>
#include FT_FREETYPE_H  

#define SINGLETON(cls)\
public:\
	static cls * GetInstance()\
	{\
		static cls * instance = nullptr;\
		if (!instance)\
		{\
			instance = new cls();\
		}\
		return instance;\
	}\

//ƽ�й�
typedef struct stDirectLight
{
	glm::vec3 direction;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
}D_LIGHT;
//���Դ
typedef struct stPointLight
{
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	GLfloat constant;
	GLfloat linear;
	GLfloat quadratic;
}PT_LIGHT;
//�۹��
typedef struct strSpotLight
{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	glm::vec3 position;
	glm::vec3 lightDir;
	GLfloat cutoff;
	GLfloat outCutoff;
	GLfloat constant;
	GLfloat linear;
	GLfloat quadratic;
}S_LIGHT;

typedef struct stMaterial
{
	GLuint texDiffuse;
	GLuint texSpecular;
	GLfloat shininess;
}MATERIAL;

typedef struct stVertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texcoords;
}VERTEX;

typedef struct stTexture
{
	GLuint id;
	string type;
	string path;
}TEXTURE;

static GLuint LoadTexture(const char * _path, bool _alpha)
{
	GLuint texture = 0;
	GLubyte * source = nullptr;
	GLint type = _alpha ? GL_RGBA : GL_RGB;
	GLint loadType = _alpha ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB;
	int width, height;
	source = SOIL_load_image(_path, &width, &height, nullptr, loadType);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, type, GL_UNSIGNED_BYTE, source);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(source);
	source = nullptr;
	return texture;
}

struct stCharacter
{
	GLuint textureID;	//��������id
	glm::ivec2 size;	//���δ�С
	glm::ivec2 bearing;	//�ӻ�׼�ߵ�������/������ƫ��ֵ
	GLuint     advance;    // ԭ�����һ������ԭ��ľ���
};

#endif