
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

//********************************************

//天空盒
void RenderSkyBox();

//模型
CMyModel * model = nullptr;
void CreateModel();
void UpdateModel(GLfloat _dt);
void RenderModel();

//绘制点光源
glm::vec3 lightPos;
glm::vec3 lightDir;
GLuint modelShaderProgram = 0;
void RenderLightPoint();

//绘制光线
void RenderLightLine();
//********************************************
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
		glClearColor(0.0f, 0.5f, 0.5f, 1.0f);
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
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);

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
	UpdateModel(_dt);
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
	//绘制点光源
	RenderLightPoint();
	//绘制光线
	RenderLightLine();
	//绘制模型
	RenderModel();
	//绘制天空盒
	//RenderSkyBox();
}

void InitObject()
{
	//初始化相机
	camera = new CMyCamera(45.0f, WIN_WIDTH, WIN_HEIGHT, glm::vec3(0.0f, 1.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	lightPos = glm::vec3(0.0f, 0.0f, 3.0f);
	lightDir = glm::normalize(-lightPos);
	CreateModel();
}

void RenderSkyBox()
{
	static GLuint skyBoxVAO = 0;
	static GLuint skyBoxVBO = 0;
	static GLuint skyBoxShaderProgram = 0;
	static GLuint texSkyBox = 0;
	static glm::mat4 matView;
	if (!skyBoxVAO)
	{

		GLfloat _vertices[] = {
			// Positions          
			-0.1f, 0.1f, -0.1f,
			-0.1f, -0.1f, -0.1f,
			0.1f, -0.1f, -0.1f,
			0.1f, -0.1f, -0.1f,
			0.1f, 0.1f, -0.1f,
			-0.1f, 0.1f, -0.1f,

			-0.1f, -0.1f, 0.1f,
			-0.1f, -0.1f, -0.1f,
			-0.1f, 0.1f, -0.1f,
			-0.1f, 0.1f, -0.1f,
			-0.1f, 0.1f, 0.1f,
			-0.1f, -0.1f, 0.1f,

			0.1f, -0.1f, -0.1f,
			0.1f, -0.1f, 0.1f,
			0.1f, 0.1f, 0.1f,
			0.1f, 0.1f, 0.1f,
			0.1f, 0.1f, -0.1f,
			0.1f, -0.1f, -0.1f,

			-0.1f, -0.1f, 0.1f,
			-0.1f, 0.1f, 0.1f,
			0.1f, 0.1f, 0.1f,
			0.1f, 0.1f, 0.1f,
			0.1f, -0.1f, 0.1f,
			-0.1f, -0.1f, 0.1f,

			-0.1f, 0.1f, -0.1f,
			0.1f, 0.1f, -0.1f,
			0.1f, 0.1f, 0.1f,
			0.1f, 0.1f, 0.1f,
			-0.1f, 0.1f, 0.1f,
			-0.1f, 0.1f, -0.1f,

			-0.1f, -0.1f, -0.1f,
			-0.1f, -0.1f, 0.1f,
			0.1f, -0.1f, -0.1f,
			0.1f, -0.1f, -0.1f,
			-0.1f, -0.1f, 0.1f,
			0.1f, -0.1f, 0.1f
		};
		//VAO VBO
		glGenVertexArrays(1, &skyBoxVAO);
		glGenBuffers(1, &skyBoxVBO);
		glGenTextures(1, &texSkyBox);

		glBindVertexArray(skyBoxVAO);
		glBindBuffer(GL_ARRAY_BUFFER, skyBoxVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(_vertices), _vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 3, 0);
		glBindVertexArray(0);

		//tex
		vector<string> names;
		names.emplace_back("../bin/res/skyBox/right.jpg");
		names.emplace_back("../bin/res/skyBox/left.jpg");
		names.emplace_back("../bin/res/skyBox/top.jpg");
		names.emplace_back("../bin/res/skyBox/bottom.jpg");
		names.emplace_back("../bin/res/skyBox/back.jpg");
		names.emplace_back("../bin/res/skyBox/front.jpg");
		
		glBindTexture(GL_TEXTURE_CUBE_MAP, texSkyBox);
		GLubyte * source = nullptr;
		GLint width, height;
		for (int i = 0; i < names.size(); ++i)
		{
			source = SOIL_load_image(names[i].c_str(), &width, &height, nullptr, SOIL_LOAD_RGB);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, source);
		}
		//设置纹理的属性
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		//创建
		skyBoxShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/skyBox.vert", "../bin/shaders/skyBox.frag");
	}

	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);
	glUseProgram(skyBoxShaderProgram);
	//重新计算相机矩阵,移除了移动，永远保证在中央
	matView = glm::mat4(glm::mat3(*camera->GetView()));

	glUniform1i(glGetUniformLocation(skyBoxShaderProgram, "texSkyBox"), 0);
	glUniformMatrix4fv(glGetUniformLocation(skyBoxShaderProgram, "matView"), 1, GL_FALSE, glm::value_ptr(matView));
	glUniformMatrix4fv(glGetUniformLocation(skyBoxShaderProgram, "matProjection"), 1, GL_FALSE, glm::value_ptr(*camera->GetProjection()));

	glBindVertexArray(skyBoxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texSkyBox);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glBindVertexArray(0);	
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
}

void CreateModel()
{
	modelShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/model.vert", "../bin/shaders/model.frag");
	model = new CMyModel("../bin/res/nanosuit_reflection/nanosuit.obj");
	model->SetCamera(camera);
	model->SetScale(glm::vec3(0.2f));
	model->SetPosition(glm::vec3(0.0f, -0.5f, 0.0f));
	model->SetRotation(glm::vec3(0.0f, 1.0f, 0.0f), 180);
	model->SetLightDir(lightDir);
}

void UpdateModel(GLfloat _dt)
{
	model->Update(_dt);
}

void RenderModel()
{
	model->Draw(modelShaderProgram);
}

void RenderLightPoint()
{
	static CMyCube * lightCube = nullptr;
	static GLuint lightShaderProgram = 0;
	if (!lightCube)
	{
		lightCube = new CMyCube();
		lightCube->SetPosition(lightPos);
		lightCube->SetScale(glm::vec3(0.1f));
		lightCube->Update(0);
		lightCube->SetCamera(camera);
		lightShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/ptLight.vert", "../bin/shaders/ptLight.frag");
	}
	lightCube->Draw(lightShaderProgram);
}

void RenderLightLine()
{
	static GLuint VAO = 0;
	static GLuint VBO = 0;
	static GLuint lightLineShaderProgram = 0;
	if (!VAO)
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* 3, nullptr, GL_STATIC_DRAW);
		glBindVertexArray(0);

		lightLineShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/lightLine.vert", "../bin/shaders/lightLine.geo", "../bin/shaders/lightLine.frag");
	}

	glUseProgram(lightLineShaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(lightLineShaderProgram, "matProjection"), 1, GL_FALSE, glm::value_ptr(*camera->GetProjection()));
	glUniformMatrix4fv(glGetUniformLocation(lightLineShaderProgram, "matView"), 1, GL_FALSE, glm::value_ptr(*camera->GetView()));
	glUniform3fv(glGetUniformLocation(lightLineShaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));
	glUniform3fv(glGetUniformLocation(lightLineShaderProgram, "lightDir"), 1, glm::value_ptr(lightDir));
	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, 1);
	glBindVertexArray(0);
}