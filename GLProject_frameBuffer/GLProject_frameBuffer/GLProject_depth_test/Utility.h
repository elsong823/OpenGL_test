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
//#include <algorithm>
using namespace std;
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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

//平行光
typedef struct stDirectLight
{
	glm::vec3 direction;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
}D_LIGHT;
//点光源
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
//聚光灯
typedef struct strSpotLight
{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
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

typedef struct stCube
{
	GLuint VAO;
	GLuint VBO;
	MATERIAL material;
	glm::vec3 position;
	glm::vec3 scale;
}MYCUBE;

typedef struct stPlane
{
	GLuint VAO;
	GLuint VBO;
	GLuint texture;
	glm::vec3 position;
	glm::vec3 scale;
}MYPLANE;

typedef struct stVegetation
{
	GLuint VAO;
	GLuint VBO;
	GLuint texture;
}VEGETATION;

static GLuint LoadTexture(GLchar * _path, GLboolean _alpha, GLint _wrapType)
{
	GLuint texture = 0;
	GLubyte * source = nullptr;
	GLint width, height;
	GLint format = _alpha ? GL_RGBA : GL_RGB;
	GLint loadFormat = _alpha ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB;
	glGenTextures(1, &texture);
	source = SOIL_load_image(_path, &width, &height, nullptr, loadFormat);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, source);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, _wrapType);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, _wrapType);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	SOIL_free_image_data(source);
	source = nullptr;

	return texture;
}

#endif