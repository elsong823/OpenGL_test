#include "Utility.h"
#include "PointLight.h"
#include "MyCamera.h"
#include "MyCube.h"

GLFWwindow * window = nullptr;
const GLint WIN_WIDTH = 1024;
const GLint WIN_HEIGHT = 768;
const GLint FRAME_NUM = 60;
GLint drawMode = GL_FILL;

//MyCamera
CMyCamera * camera = nullptr;

//MyLight
glm::vec3 directLight = glm::vec3(-0.2f, -1.0f, -0.3f);
vector<CPointLight*> pointLights;
//My Cube
vector<CMyCube*> cubes;


void KeyCallback(GLFWwindow * _win, int _key, int _scanCode, int _action, int _mode);
void MouseMoveCallback(GLFWwindow * _win, double _offsetX, double _offsetY);
void ScrollCallback(GLFWwindow * _win, double _offsetX, double _offsetY);
void Update(GLfloat _dt);
void Draw();
void Destroy();
void InitObjects();

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "Learn openGL lighting...", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseMoveCallback);
	glfwSetScrollCallback(window, ScrollCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewExperimental = GL_TRUE;
	if (glewInit())
	{
		printf_s("Init glew failed.\n");
		Destroy();
		return -1;
	}
	glEnable(GL_DEPTH_TEST);

	GLint _width, _height;
	glfwGetFramebufferSize(window, &_width, &_height);
	glViewport(0, 0, _width, _height);

	InitObjects();

	GLfloat deltaTime = 1.0f / FRAME_NUM;
	GLfloat lastTime = glfwGetTime();
	GLfloat curTime = glfwGetTime() + deltaTime + 0.001f;
	GLfloat elapsedTime = 0.0f;
	while (!glfwWindowShouldClose(window))
	{
		curTime = glfwGetTime();
		elapsedTime = curTime - lastTime;
		if (elapsedTime < deltaTime)
			continue;
		lastTime = curTime;
		//printf_s("frame:%.1f\n", 1.0f / elapsedTime);
		glPolygonMode(GL_FRONT_AND_BACK, drawMode);
		glfwPollEvents();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Update(elapsedTime);
		Draw();

		glfwSwapBuffers(window);
	}

	Destroy();
	return 0;
}

void Update(GLfloat _dt)
{
	static GLfloat _timer = 0.0f;
	_timer += _dt;
	camera->Update(_dt);
	//Update light pos
	auto _posX = sin(glfwGetTime()) * 2.0f;
	auto _posZ = cos(glfwGetTime()) * 2.0f;
	auto _posY = 0.3f;
	//light->SetPosition(glm::vec3(_posX, _posY, _posZ));

	//light->Update(_dt);
	for (auto _cube : cubes)
	{
		//_cube->SetRotation(glm::vec3(0.0f, 1.0f, 0.0f), _timer * 50.f);
		_cube->Update(_dt);
	}
}

void Draw()
{
	//Draw Light
	for (auto _light : pointLights)
	{
		_light->Draw();
	}

	//Draw Cube
	for (auto _cube : cubes)
	{
		_cube->Draw();
	}

}

void InitObjects()
{
	//Camera
	camera = new CMyCamera(45.0f, WIN_WIDTH, WIN_HEIGHT,
		glm::vec3(0.0f, 0.0f, 2.0f),
		glm::vec3(0.0f, 0.0f, 0.0f)
		);

	//Direct light
	//CMyCube::SetLightDir(-0.2f, -1.0f, -0.3f);

	//Point light
	float linear, quadratic;
	linear = 0.5f;
	quadratic = 0.9f;
	auto _light = new CPointLight(camera, glm::vec3(0.0f, 1.0f, -1.5f), linear, quadratic, glm::vec3(1.0f,1.0f,1.0f));
	pointLights.push_back(_light);

	_light = new CPointLight(camera, glm::vec3(1.0f, -1.0f, -0.5f), linear, quadratic, glm::vec3(1.0f));
	pointLights.push_back(_light);

	_light = new CPointLight(camera, glm::vec3(2.0f, 0.3f, -3.6f), linear, quadratic, glm::vec3(1.0f));
	//pointLights.push_back(_light);

	_light = new CPointLight(camera, glm::vec3(3.3f, -0.4f, -5.2f), linear, quadratic, glm::vec3(1.0f));
	pointLights.push_back(_light);

	CMyCube::m_pointLights = &pointLights;
	//Cubes:
	int num = 6;
	for (int i = 0; i < num; ++i)
	{
		float _x = i * 1.1f;
		float _y = 0.0f;
		float _z = -_x;
		auto cube = new CMyCube(camera);
		cube->SetShader("../bin/Shaders/cube1.vert", "../bin/Shaders/cube1.frag");
		cube->SetPosition(glm::vec3(_x, _y, _z));
		cubes.push_back(cube);
		//cube->SetScale(glm::vec3(0.2f));
		//cube->SetLight(_light);
	}
}

void Destroy()
{
	//Destroy cubes
	for (auto _cube : cubes)
	{
		//...
	}
	glfwTerminate();
}

void KeyCallback(GLFWwindow * _win, int _key, int _scanCode, int _action, int _mode)
{
	if (_key == GLFW_KEY_ESCAPE && _action == GLFW_RELEASE)
	{
		glfwSetWindowShouldClose(_win, GLFW_TRUE);
	}
	else if (_key == GLFW_KEY_1 && _action == GLFW_RELEASE)
	{
		drawMode = GL_FILL;
	}
	else if (_key == GLFW_KEY_2 && _action == GLFW_RELEASE)
	{
		drawMode = GL_LINE;
	}
	else if (_key == GLFW_KEY_3 && _action == GLFW_RELEASE)
	{
		for (auto _cube : cubes)
		{
			_cube->AddShininess();
		}
	}
	else if (_key == GLFW_KEY_4 && _action == GLFW_RELEASE)
	{
		for (auto _cube : cubes)
		{
			_cube->ReduceShininess();
		}
	}
	else if (_key == GLFW_KEY_W && camera)
	{
		if (_action == GLFW_PRESS)
		{
			camera->MoveCamera(true, CMD_FORWARD);
		}
		else if (_action == GLFW_RELEASE)
		{
			camera->MoveCamera(false, CMD_FORWARD);
		}
	}
	else if (_key == GLFW_KEY_S && camera)
	{
		if (_action == GLFW_PRESS)
		{
			camera->MoveCamera(true, CMD_BACKWARD);
		}
		else if (_action == GLFW_RELEASE)
		{
			camera->MoveCamera(false, CMD_BACKWARD);
		}
	}
	else if (_key == GLFW_KEY_A && camera)
	{
		if (_action == GLFW_PRESS)
		{
			camera->MoveCamera(true, CMD_LEFT);
		}
		else if (_action == GLFW_RELEASE)
		{
			camera->MoveCamera(false, CMD_LEFT);
		}
	}
	else if (_key == GLFW_KEY_D && camera)
	{
		if (_action == GLFW_PRESS)
		{
			camera->MoveCamera(true, CMD_RIGHT);
		}
		else if (_action == GLFW_RELEASE)
		{
			camera->MoveCamera(false, CMD_RIGHT);
		}
	}
	else if (_key == GLFW_KEY_Q && camera)
	{
		if (_action == GLFW_PRESS)
		{
			camera->MoveCamera(true, CMD_UPWARD);
		}
		else if (_action == GLFW_RELEASE)
		{
			camera->MoveCamera(false, CMD_UPWARD);
		}
	}
	else if (_key == GLFW_KEY_E && camera)
	{
		if (_action == GLFW_PRESS)
		{
			camera->MoveCamera(true, CMD_DOWNWARD);
		}
		else if (_action == GLFW_RELEASE)
		{
			camera->MoveCamera(false, CMD_DOWNWARD);
		}
	}
}

void MouseMoveCallback(GLFWwindow * _win, double _offsetX, double _offsetY)
{
	static double lastOffsetX = 0.0f;
	static double lastOffsetY = 0.0f;
	static bool firstMove = true;
	if (firstMove)
	{
		lastOffsetX = _offsetX;
		lastOffsetY = _offsetY;
		firstMove = false;
	}
	auto _x = _offsetX - lastOffsetX;
	auto _y = _offsetY - lastOffsetY;
	camera->SetYaw(_x);
	camera->SetPitch(_y);
	lastOffsetX = _offsetX;
	lastOffsetY = _offsetY;
}

void ScrollCallback(GLFWwindow * _win, double _offsetX, double _offsetY)
{
	if (camera)
	{
		camera->ZoomCamera(-_offsetY);
	}
}