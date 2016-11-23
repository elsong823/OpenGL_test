#include "Utility.h"
#include "Camera.h"
#include "MyCube.h"

GLint drawMode = GL_FILL;
GLint FramNum = 60;

glm::vec3 vecCamPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 vecCamUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 vecCamFront = glm::vec3(0.0f, 0.0f, -1.0f);

bool keyStatus[1024] = {};
GLfloat aspect = 45.0f;
CCamera * camera = nullptr;

void KeyCallback(GLFWwindow * _win, int _key, int _scancode, int _action, int _mode);
void MouseCallback(GLFWwindow * _win, double _posX, double _posY);
void ScrollCallback(GLFWwindow * _win, double _xOffset, double _yOffset);

void CameraMove(GLfloat _elapsedTime);

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	auto window = glfwCreateWindow(800, 600, "Strat learning openGL", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetScrollCallback(window, ScrollCallback);
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		glfwTerminate();
		return -1;
	}
	int _winWidth, _winHeight;
	glfwGetWindowSize(window, &_winWidth, &_winHeight);

	//camera
	camera = new CCamera(_winWidth, _winHeight, glm::vec3(0.0f, 3.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	//mat
	const glm::mat4 * matView = camera->GetViewMat();
	const glm::mat4 * matProjection = camera->GetProjectionMat();

	CMyCube * _cubes[10] = {};
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(2.0f, 5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f, 3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f, 2.0f, -2.5f),
		glm::vec3(1.5f, 0.2f, -1.5f),
		glm::vec3(-1.3f, 1.0f, -1.5f)
	};
	int _len = sizeof(cubePositions) / sizeof(glm::vec3);
	srand((unsigned int)time(nullptr));
	for (int i = 0; i < _len; i++)
	{
		auto _cube = new CMyCube("../bin/Shaders/texture1.vs", "../bin/Shaders/texture1.frag", camera);
		_cube->SetPosition(cubePositions[i].x, cubePositions[i].y, cubePositions[i].z);
		_cubes[i] = _cube;

		//set Rotate Axis
		if (i % 3 == 0)
		{
			auto _x = rand() % 10 + 1;
			auto _y = rand() % 10 + 1;
			auto _z = rand() % 10 + 1;
			_cube->SetRotateAxis(_x, _y, _z);
		}
	}

	glEnable(GL_DEPTH_TEST);

	GLfloat lastTime = glfwGetTime();
	GLfloat curTime = glfwGetTime();
	GLfloat deltaTime = 1.0f / FramNum;
	GLfloat elapsedTime = 0.0f;
	while (!glfwWindowShouldClose(window))
	{
		curTime = glfwGetTime();
		elapsedTime = curTime - lastTime;
		if (elapsedTime < deltaTime)
			continue;

		lastTime = curTime;
		CameraMove(elapsedTime);

		glfwPollEvents();
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glPolygonMode(GL_FRONT_AND_BACK, drawMode);

		for (int i = 0; i < _len; i++)
		{
			_cubes[i]->Draw(elapsedTime);
		}

		glfwSwapBuffers(window);
	}


	return 0;
}

void KeyCallback(GLFWwindow * _win, int _key, int _scancode, int _action, int _mode)
{
	if (_key == GLFW_KEY_ESCAPE && _action == GLFW_RELEASE)
	{
		glfwSetWindowShouldClose(_win, GL_TRUE);
	}
	else if (_key == GLFW_KEY_1 && _action == GLFW_RELEASE)
	{
		drawMode = GL_FILL;
	}
	else if (_key >= 0 && _key <= 1024)
	{
		if (_action == GLFW_PRESS)
		{
			keyStatus[_key] = true;
		}
		else if (_action == GLFW_RELEASE)
		{
			keyStatus[_key] = false;
		}
	}

}

GLboolean firstMouse = false;
void MouseCallback(GLFWwindow * _win, double _posX, double _posY)
{
	static GLfloat lastOffsetX = 0.0f;
	static GLfloat lastOffsetY = 0.0f;
	if (!firstMouse)
	{
		lastOffsetX = _posX;
		lastOffsetY = _posY;
		printf_s("first:%.2f, %.2f\n", lastOffsetX, lastOffsetY);
		firstMouse = true;
	}
	camera->RotateCamera(lastOffsetX - _posX, -lastOffsetY + _posY);
	lastOffsetX = _posX;
	lastOffsetY = _posY;
}

void ScrollCallback(GLFWwindow * _win, double _xOffset, double _yOffset)
{
	camera->CameraZoom(_yOffset);
}

void CameraMove(GLfloat _elapsedTime)
{
	if (!camera)
		return;

	GLfloat cameraMoveSpeed = 2;
	if (keyStatus[GLFW_KEY_W])
	{
		camera->MoveCamera(CMD_FORWARD, _elapsedTime);
	}
	if (keyStatus[GLFW_KEY_S])
	{
		camera->MoveCamera(CMD_BACKWARD, _elapsedTime);
	}
	if (keyStatus[GLFW_KEY_A])
	{
		camera->MoveCamera(CMD_LEFT, _elapsedTime);
	}
	if (keyStatus[GLFW_KEY_D])
	{
		camera->MoveCamera(CMD_RIGHT, _elapsedTime);
	}
}