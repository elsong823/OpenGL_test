
#include "Utility.h"
#include "MyBox.h"
#include "MyShader.h"
#include "MyMesh.h"
#include "MyModel.h"
#include "LightLine.h"
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


//******************************

//墙有关
GLuint wallVAO = 0;
GLuint wallVBO = 0;
GLuint wallShaderProgram = 0;
GLuint texWall = 0;
GLuint texWallNormal = 0;
GLuint texWallParallax = 0;
GLfloat rotateDegree = 0.0f;	
GLint useParallax = 0;
glm::vec3 tangent;
glm::vec3 bitangent;
void CreateWall();
void UpdateWall(GLfloat _dt);
void DrawWall();

//光
glm::vec3 lightPoint(0.0f, 1.0f, 1.2f);
CLightLine * lightLine = nullptr;
void DrawLight();
void DrawLightLine();
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
	
	//glEnable(GL_CULL_FACE);
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
	else if (_key == GLFW_KEY_UP && _action == GLFW_RELEASE)
	{
		rotateDegree += 3.0f;
		printf_s("degree : %.0f\n", rotateDegree);
	}
	else if (_key == GLFW_KEY_DOWN && _action == GLFW_RELEASE)
	{
		rotateDegree -= 3.0f;
		printf_s("degree : %.0f\n", rotateDegree);
	}
	else if (_key == GLFW_KEY_0 && _action == GLFW_RELEASE)
	{
		if (useParallax == 0)
		{
			useParallax = 1;
			printf_s("Use parallax!\n");
		}
		else
		{
			useParallax = 0;
			printf_s("Unuse parallax!\n");
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
	UpdateWall(_dt);
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
	DrawLight();
	DrawLightLine();
	DrawWall();
}

void InitObject()
{
	//初始化相机
	camera = new CMyCamera(45.0f, WIN_WIDTH, WIN_HEIGHT, glm::vec3(0.0f, 1.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	CreateWall();
}

void CreateWall()
{
	//设置顶点
	GLfloat vertices[] = {
		-1.0f,  1.0f,  0.0f,  1.0f,
		-1.0f, -1.0f,  0.0f,  0.0f,
		 1.0f,  1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,  0.0f
	};
	glGenVertexArrays(1, &wallVAO);
	glGenBuffers(1, &wallVBO);
	glBindVertexArray(wallVAO);
	glBindBuffer(GL_ARRAY_BUFFER, wallVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 4, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 4, (GLvoid*)(sizeof(GLfloat)* 2));
	glBindVertexArray(0);
	//读取图片
	//砖墙
	texWall = LoadTexture("../bin/res/bricks2.jpg", false);
	texWallNormal = LoadTexture("../bin/res/bricks2_normal.jpg", false);
	texWallParallax = LoadTexture("../bin/res/bricks2_disp.jpg", false);
	//木头
	//texWall = LoadTexture("../bin/res/wood.png", true);
	//texWallNormal = LoadTexture("../bin/res/toy_box_normal.png", true);
	//texWallParallax = LoadTexture("../bin/res/toy_box_disp.png", true);
	//创建shader
	wallShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/wall.vert", "../bin/shaders/wall.frag");
	//计算tangent、bitangent
	glm::vec3 p1(-2.0f,-2.0f, 0.0f);
	glm::vec3 p2(-2.0f, 2.0f, 0.0f);
	glm::vec3 p3( 2.0f, 2.0f, 0.0f);
	glm::vec2 uv1(0.0f, 0.0f);
	glm::vec2 uv2(0.0f, 2.0f);
	glm::vec2 uv3(2.0f, 2.0f);
	glm::vec3 e1 = p2 - p1;
	glm::vec3 e2 = p3 - p1;
	glm::vec2 d1 = uv2 - uv1;
	glm::vec2 d2 = uv3 - uv1;
	GLfloat f = 1.0f / ((d1.x*d2.y) - (d2.x*d1.y));
	tangent.x = f * (d2.y * e1.x - d1.y * e2.x);
	tangent.y = f * (d2.y * e1.y - d1.y * e2.y);
	tangent.z = f * (d2.y * e1.z - d1.y * e2.z);
	tangent = glm::normalize(tangent);
	bitangent.x = f * (-d2.x * e1.x + d1.x * e2.x);
	bitangent.y = f * (-d2.x * e1.y + d1.x * e2.y);
	bitangent.z = f * (-d2.x * e1.z + d1.x * e2.z);
	bitangent = glm::normalize(bitangent);
}

void UpdateWall(GLfloat _dt)
{
	//range
	static glm::mat4 matModel;
	static float time = 0.0f;
	time += _dt;
	matModel = glm::mat4();
	matModel = glm::rotate(matModel, glm::radians(rotateDegree), glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(wallShaderProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(matModel));

}

void DrawWall()
{
	glUseProgram(wallShaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(wallShaderProgram, "matView"), 1, GL_FALSE, glm::value_ptr(*camera->GetView()));
	glUniformMatrix4fv(glGetUniformLocation(wallShaderProgram, "matProjection"), 1, GL_FALSE, glm::value_ptr(*camera->GetProjection()));
	glUniform3fv(glGetUniformLocation(wallShaderProgram, "eyePos"), 1, glm::value_ptr(*camera->GetPosition()));
	glUniform3fv(glGetUniformLocation(wallShaderProgram, "lightDir"), 1, glm::value_ptr(glm::normalize(-lightPoint)));
	glUniform3fv(glGetUniformLocation(wallShaderProgram, "tangent"), 1, glm::value_ptr(tangent));
	glUniform3fv(glGetUniformLocation(wallShaderProgram, "bitangent"), 1, glm::value_ptr(bitangent));
	glUniform1i(glGetUniformLocation(wallShaderProgram, "useParallax"), useParallax);
	glUniform1f(glGetUniformLocation(wallShaderProgram, "height_scale"), 0.1f);
	glUniform1i(glGetUniformLocation(wallShaderProgram, "texDiffuse"), 0);
	glUniform1i(glGetUniformLocation(wallShaderProgram, "texNormal"), 1);
	glUniform1i(glGetUniformLocation(wallShaderProgram, "texParallax"), 2);

	glBindVertexArray(wallVAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texWall);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texWallNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texWallParallax);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
}

void DrawLight()
{
	static CMyCube * lightCube = nullptr;
	static GLuint lightShaderProgram = 0;
	if (!lightCube)
	{
		lightCube = new CMyCube();
		lightCube->SetPosition(lightPoint);
		lightCube->SetScale(glm::vec3(0.1f));
		lightCube->SetCamera(camera);
		lightCube->Update(0);
		lightShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/light.vert", "../bin/shaders/light.frag");
	}
	lightCube->Draw(lightShaderProgram);
}

void DrawLightLine()
{
	if (!lightLine)
	{
		lightLine = new CLightLine(lightPoint, glm::vec3(0.0f), camera);
	}
	lightLine->Draw();
}