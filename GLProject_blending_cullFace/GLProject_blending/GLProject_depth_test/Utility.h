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
	glm::vec3 position;
	glm::vec3 scale;
}MYCUBE;

typedef struct stPlane
{
	GLuint VAO;
	GLuint VBO;
	glm::vec3 position;
	glm::vec3 scale;
}MYPLANE;

typedef struct stVegetation
{
	GLuint VAO;
	GLuint VBO;
	GLuint texture;
}VEGETATION;

#endif