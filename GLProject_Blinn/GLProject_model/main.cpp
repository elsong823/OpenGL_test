
#include "Utility.h"
#include "MyBox.h"
#include "MyShader.h"
#include "MyMesh.h"
#include "MyModel.h"
#include "MyCamera.h"

GLFWwindow * window = nullptr;
const GLint WIN_WIDTH = 800;
const GLint WIN_HEIGHT = 600;
const GLint FRAME = 60;

bool keyStatus[128] = {};
GLint drawMode = GL_FILL;
CMyCamera * camera = nullptr;

void Init();
void InitObject();
void Exit();
void Draw();
void Update(GLfloat _dt);
void CheckKeyStatus();
void KeyCallback(GLFWwindow * _win, int _key, int _scanCode, int _action, int _mode);
void CursorCallback(GLFWwindow * _win, double _offsetX, double _offsetY);

//*****************************

GLuint planeVAO = 0;
GLuint planeVBO = 0;
GLuint texPlane = 0;
GLuint planeShaderProgram = 0;
CMyCube * pointLight = nullptr;
void CreatePlane();
void DrawPlane();
GLuint UBO;
GLuint cubeShaderProgram = 0;
void CreateUBO();
void DrawLight();

bool blinn = false;

//******************************

int main()
{
	Init();

	GLfloat frameTime = 1.0f / FRAME;
	GLfloat curTime = glfwGetTime();
	GLfloat lastTime = glfwGetTime() - frameTime;
	GLfloat elapsedTime = 0.0f;
	while (!glfwWindowShouldClose(window))
	{
		curTime = glfwGetTime();
		elapsedTime = curTime - lastTime;
		if (elapsedTime < frameTime)
			continue;

		//printf_s("%.5f\n",1.0f / elapsedTime);
		lastTime = curTime;
		glfwPollEvents();
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Update(elapsedTime);
		Draw();
		
		glfwSwapBuffers(window);
	}

	Exit();
	return 0;
}

void Init()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "OpenGL_model",nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	GLint _width, _height;
	glfwGetFramebufferSize(window, &_width, &_height);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, CursorCallback);
	glewExperimental = GL_TRUE;
	glewInit();
	glViewport(0, 0, _width, _height);
	glEnable(GL_DEPTH_TEST);
	//初始化对象
	InitObject();
}

void Exit()
{
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
	//设置按键
	else if (_key >= GLFW_KEY_A && _key <= GLFW_KEY_Z)
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
	else if (_key == GLFW_KEY_5 && _action == GLFW_RELEASE)
	{
		blinn = !blinn;
		printf_s("blinn = %d\n", blinn);
	}
}

void CursorCallback(GLFWwindow * _win, double _offsetX, double _offsetY)
{
	static double lastX, lastY;
	static bool firstMove = true;
	if (firstMove)
	{
		lastX = _offsetX;
		lastY = _offsetY;
		firstMove = false;
	}
	double moveX = _offsetX - lastX;
	double moveY = _offsetY - lastY;
	camera->RotateCamera((GLfloat)moveX, (GLfloat)moveY);
	lastX = _offsetX;
	lastY = _offsetY;
}

void Update(GLfloat _dt)
{
	CheckKeyStatus();
	camera->Update(_dt);
	pointLight->Update(_dt);
	//
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 16 * 4, glm::value_ptr(*camera->GetView()));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void CheckKeyStatus()
{
	if (keyStatus[GLFW_KEY_W])
	{
		camera->MoveCamera(CMD_FORWARD, true);
	}
	else
	{
		camera->MoveCamera(CMD_FORWARD, false);
	}
	if (keyStatus[GLFW_KEY_S])
	{
		camera->MoveCamera(CMD_BACKWARD, true);
	}
	else
	{
		camera->MoveCamera(CMD_BACKWARD, false);
	}
	if (keyStatus[GLFW_KEY_A])
	{
		camera->MoveCamera(CMD_LEFT, true);
	}
	else
	{
		camera->MoveCamera(CMD_LEFT, false);
	}
	if (keyStatus[GLFW_KEY_D])
	{
		camera->MoveCamera(CMD_RIGHT, true);
	}
	else
	{
		camera->MoveCamera(CMD_RIGHT, false);
	}
	if (keyStatus[GLFW_KEY_Q])
	{
		camera->MoveCamera(CMD_UP, true);
	}
	else
	{
		camera->MoveCamera(CMD_UP, false);
	}
	if (keyStatus[GLFW_KEY_E])
	{
		camera->MoveCamera(CMD_DOWN, true);
	}
	else
	{
		camera->MoveCamera(CMD_DOWN, false);
	}
}

void Draw()
{
	glPolygonMode(GL_FRONT_AND_BACK, drawMode);
	DrawPlane();
	DrawLight();
}

void InitObject()
{
	//初始化相机
	camera = new CMyCamera(45.0f, WIN_WIDTH, WIN_HEIGHT, glm::vec3(0.0f, 1.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	CreatePlane();
	CreateUBO();
}

void CreatePlane()
{
	GLfloat planeVertices[] = {
		// Positions            // Texture Coords (note we set these higher than 1 that together with GL_REPEAT as texture wrapping mode will cause the floor texture to repeat)
		-2.0f, -0.5f, -2.0f, 0.0f, 1.0f, 0.0f, 0.0f, 2.0f,
		-2.0f, -0.5f,  2.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		 2.0f, -0.5f,  2.0f, 0.0f, 1.0f, 0.0f, 2.0f, 0.0f,

		-2.0f, -0.5f, -2.0f, 0.0f, 1.0f, 0.0f, 0.0f, 2.0f,
		 2.0f, -0.5f,  2.0f, 0.0f, 1.0f, 0.0f, 2.0f, 0.0f,
		 2.0f, -0.5f, -2.0f, 0.0f, 1.0f, 0.0f, 2.0f, 2.0f
	};
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 8, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 8, (GLvoid*)(sizeof(GLfloat)* 3));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 8, (GLvoid*)(sizeof(GLfloat)* 6));
	glBindVertexArray(0);
	//shader
	planeShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/plane.vert","../bin/shaders/plane.frag");
	auto idx = glGetUniformBlockIndex(planeShaderProgram, "Matrices");
	glUniformBlockBinding(planeShaderProgram, idx, 0);
	//Tex
	texPlane = LoadTexture("../bin/res/container2.png");
	//Create cube
	pointLight = new CMyCube();
	pointLight->SetPosition(glm::vec3(1.0f, 0.5f, 1.3f));
	pointLight->SetScale(glm::vec3(0.1f));
	cubeShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/cube.vert", "../bin/shaders/cube.frag");
	idx = glGetUniformBlockIndex(cubeShaderProgram, "Matrices");
	glUniformBlockBinding(cubeShaderProgram, idx, 0);
}

void DrawPlane()
{

	glEnable(GL_SCISSOR_TEST);
	glScissor(0, 0, 400, 300);
	glUseProgram(planeShaderProgram);
	glBindVertexArray(planeVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texPlane);

	static glm::mat4 matModel;
	glUniformMatrix4fv(glGetUniformLocation(planeShaderProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(matModel));
	glUniform1i(glGetUniformLocation(planeShaderProgram, "u_fragTexture"), 0);
	//设置点光源的位置
	glUniform3fv(glGetUniformLocation(planeShaderProgram, "lightPos"), 1, glm::value_ptr(pointLight->GetPosition()));
	//设置摄像机(眼睛的位置)
	glUniform3fv(glGetUniformLocation(planeShaderProgram, "eyePosition"), 1, glm::value_ptr(*camera->GetPosition()));
	glUniform1i(glGetUniformLocation(planeShaderProgram, "blinn"), blinn);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glDisable(GL_SCISSOR_TEST);
	//0,0,400,300
	GLfloat param[4];
	glGetFloatv(GL_SCISSOR_BOX, param);
	int a = 1;
}

void CreateUBO()
{
	glGenBuffers(1, &UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4)*2, nullptr, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 16 * 4, 16 * 4, glm::value_ptr(*camera->GetProjection()));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void DrawLight()
{
	pointLight->Draw(cubeShaderProgram);
}