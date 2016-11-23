
#ifndef GLEW_STATIC
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
using namespace std;

#define SINGLETON(className) \
public:\
	static className * GetInstance()\
{\
	static className * instance = nullptr;\
if (!instance)\
	{\
	instance = new className(); \
	}\
	return instance;\
}\

#define DELETE_POINTER(ptr) if(ptr){delete ptr; ptr = nullptr;}
#define DELETE_ARRAY(ptr) if(ptr){delete[] ptr; ptr = nullptr;}

#endif