
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

//************************************

GLuint UBO;
void CreateUBO();
void UpdateUBO();

GLuint cardVAO;
GLuint cardVBO;
GLuint instanceVBO;
GLuint cardShaderProgram;
glm::vec2 cardPos[100];
void CreateCards();
void DrawCards();

//小行星
const int planetNum = 1000;
GLuint planetShaderProgram = 0;
GLuint rockShaderProgram = 0;
glm::mat4 * modelMatrices;
CMyModel * planet = nullptr;
CMyModel * rock = nullptr;
void CreatePlanets();
void UpdatePlanets(GLfloat _dt);
void DrawPlanets();

//************************************


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

		printf_s("time:%.5f\n",1.0f / elapsedTime);
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
	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

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
	UpdateUBO();
	UpdatePlanets(_dt);
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
	DrawPlanets();
}

void InitObject()
{
	//初始化相机
	camera = new CMyCamera(45.0f, WIN_WIDTH, WIN_HEIGHT, glm::vec3(0.0f, 1.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	CreateUBO();
	//初始化shader
	planetShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/planet.vert", "../bin/shaders/model.frag");
	rockShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/rock.vert", "../bin/shaders/model.frag");
	auto idx = glGetUniformBlockIndex(planetShaderProgram, "Matrices");
	glUniformBlockBinding(planetShaderProgram, idx, 0);
	CreatePlanets();
}

void CreateCards()
{
	//Create Shader
	cardShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/card.vert", "../bin/shaders/card.frag");
	//Init pos
	for (int x = 0; x < 10; ++x)
	{
		for (int y = 0; y < 10; ++y)
		{
			cardPos[x * 10 + y].x = (x - 5) * 0.2f + 0.1f;
			cardPos[x * 10 + y].y = (y - 5) * 0.2f + 0.1f;	
		}
	}
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 100, &cardPos[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	GLfloat quadVertices[] = {
		//  ---位置---   ------颜色-------
		-0.05f, 0.05f, 1.0f, 0.0f, 0.0f,
		-0.05f, -0.05f, 0.0f, 0.0f, 1.0f,
		0.05f, -0.05f, 0.0f, 1.0f, 0.0f,

		-0.05f, 0.05f, 1.0f, 0.0f, 0.0f,
		0.05f, -0.05f, 0.0f, 1.0f, 0.0f,
		0.05f, 0.05f, 0.0f, 1.0f, 1.0f
	};
	glGenVertexArrays(1, &cardVAO);
	glGenBuffers(1, &cardVBO);

	glBindVertexArray(cardVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cardVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, (sizeof(GLfloat)) * 5, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (sizeof(GLfloat)) * 5, (GLvoid*)((sizeof(GLfloat)) * 3));
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, (sizeof(GLfloat)) * 2, 0);
	glVertexAttribDivisor(2, 1);
	glBindVertexArray(0);
	
}	

void DrawCards()
{
	glUseProgram(cardShaderProgram);
	char name[32] = {};
	for (int i = 0; i < 100; ++i)
	{
		sprintf_s(name, 32, "offsets[%d]", i);
		glUniform2fv(glGetUniformLocation(cardShaderProgram, name), 1, glm::value_ptr(cardPos[i]));
	}
	glBindVertexArray(cardVAO);
	//glm::mat4 matModel;
	//for (int i = 0; i < 100; ++i)
	//{
	//	matModel = glm::mat4();
	//	matModel = glm::translate(matModel, glm::vec3(cardPos[i] ,0.0f));
	//	glUniformMatrix4fv(glGetUniformLocation(cardShaderProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(matModel));
	//	glDrawArrays(GL_TRIANGLES, 0, 6);
	//}
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);
	glBindVertexArray(0);
}

void CreatePlanets()
{
	//Create model
	modelMatrices = new glm::mat4[planetNum];
	srand(glfwGetTime());
	float radius = 20.0f;
	float offset = 2.5f;
	for (int i = 0; i < planetNum; ++i)
	{
		glm::mat4 model = glm::mat4();
		GLfloat angle = i * 1.0f / planetNum * 360.0f;
		GLfloat displacement = rand() % (int)(2 * offset * 100) / 100.0f - offset;
		//Position
		glm::vec3 pos;
		pos.x = radius * cos(angle) + displacement;
		pos.y = displacement * 0.4f;
		pos.z = radius * sin(angle) + (rand() % (int)(2 * offset * 100) / 100.0f - offset);
		model = glm::translate(model, pos);
		//Scale
		GLfloat scale = rand() % 20 / 100.0f + 0.05f;
		model = glm::scale(model, glm::vec3(scale));
		//Rotation
		GLfloat rotate = rand() % 360;
		model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.4f, 0.6f, 0.8f));

		modelMatrices[i] = model;
	}
	//Create planet
	planet = new CMyModel("../bin/res/model/planet/planet.obj");

	//Create rock
	rock = new CMyModel("../bin/res/model/rock/rock.obj");
	rock->SetupInstanceData(modelMatrices, planetNum);
}

void UpdatePlanets(GLfloat _dt)
{

}

void DrawPlanets()
{
	static glm::mat4 matModel;
	//绘制行星
	if (planet)
	{
		matModel = glm::mat4();
		matModel = glm::scale(matModel, glm::vec3(0.8f));
		glUseProgram(planetShaderProgram);
		glUniformMatrix4fv(glGetUniformLocation(planetShaderProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(matModel));
		planet->Draw(planetShaderProgram);
	}
	//绘制陨石
	glUseProgram(rockShaderProgram);
	rock->Draw(0);
}

void CreateUBO()
{
	glGenBuffers(1, &UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferData(GL_UNIFORM_BUFFER, 16 * 4 * 2, nullptr, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 16 * 4, 16 * 4, glm::value_ptr(*camera->GetProjection()));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UpdateUBO()
{
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 16 * 4, glm::value_ptr(*camera->GetView()));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

