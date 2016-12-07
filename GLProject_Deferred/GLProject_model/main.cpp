
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

//***************************************************

//shaders
GLuint modelShaderProgram = 0;

//初始化部分
CMyModel * model = nullptr;
vector<glm::vec3> objPositions; //模型位置
vector<glm::vec3> lightPositions;	//光照位置
vector<glm::vec3> lightColors;	//光照颜色
void SetupWorld();

//绘制屏幕大小的四边形
GLuint quadShaderProgram = 0;
void RenderQuad();

//绘制主场景
void RenderNormalScene();

//创建G-BUFFER
GLuint gFBO = 0;
GLuint gRBO = 0;
GLuint gPosition = 0;	//位置
GLuint gNormal = 0;		//法线
GLuint gAlbedoSpec = 0;	//反射率和高光系数
void SetupGBuffer();

//延迟渲染
GLuint deferredShaderProgram = 0;
void DeferredRender();
//test
GLint showType = 4;	//4:position 5:normal 6:albedo 7:spec

GLuint lightCubeShaderProgram = 0.0f;
void DrawLightCube();

//***************************************************

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
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
	else if (_key >= GLFW_KEY_4 
			&& _key <= GLFW_KEY_7	
			&& _action == GLFW_RELEASE)
	{
		showType = _key - GLFW_KEY_0;
		printf_s("current show type : %d\n", showType);
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
	glBindFramebuffer(GL_FRAMEBUFFER, gFBO);
	glClear(GL_COLOR_BUFFER_BIT	| GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//将场景中的模型信息都渲染到FBO中
	RenderNormalScene();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//测试
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//GLuint showTex = gPosition;
	//GLboolean showAlbedo = 0;
	//GLboolean showSpec = 0;
	//switch (showType)
	//{
	//case 4:
	//	showTex = gPosition;
	//	break;
	//case 5:
	//	showTex = gNormal;
	//	break;
	//case 6:
	//	showTex = gAlbedoSpec;
	//	showAlbedo = 1;
	//	break;
	//case 7:
	//	showTex = gAlbedoSpec;
	//	showSpec = 1;
	//	break;
	//default:
	//	break;
	//}
	//glClear(GL_COLOR_BUFFER_BIT);
	//glUseProgram(quadShaderProgram);
	//glUniform1i(glGetUniformLocation(quadShaderProgram, "texSource"), 0);
	//glUniform1i(glGetUniformLocation(quadShaderProgram, "showAlbedo"), showAlbedo);
	//glUniform1i(glGetUniformLocation(quadShaderProgram, "showSpec"), showSpec);
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, showTex);
	//RenderQuad();

	//延迟输出
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DeferredRender();

	//输出灯光箱子
	DrawLightCube();
}

void InitObject()
{
	//初始化相机
	camera = new CMyCamera(45.0f, WIN_WIDTH, WIN_HEIGHT, glm::vec3(0.0f, 1.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	SetupGBuffer();
	SetupWorld();
}

void RenderQuad()
{
	static GLuint quadVAO = 0;
	static GLuint quadVBO = 0;
	if (quadVAO == 0)
	{
		GLfloat quadVertices[] = {
			// Positions        // Texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// Setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
	void SetupWorld();
}

void SetupWorld()
{
	//创建shaders
	modelShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/model.vert", "../bin/shaders/model.frag");
	quadShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/quad.vert", "../bin/shaders/quad.frag");
	deferredShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/deferredRender.vert", "../bin/shaders/deferredRender.frag");
	lightCubeShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/box.vert", "../bin/shaders/box.frag");

	//创建模型
	model = new CMyModel("../bin/res/model/nanosuit.obj");
	model->SetCamera(camera);
	
	//填充模型位置
	objPositions.push_back(glm::vec3(-3.0, -3.0, -3.0));
	objPositions.push_back(glm::vec3( 0.0, -3.0, -3.0));
	objPositions.push_back(glm::vec3( 3.0, -3.0, -3.0));
	objPositions.push_back(glm::vec3(-3.0, -3.0,  0.0));
	objPositions.push_back(glm::vec3( 0.0, -3.0,  0.0));
	objPositions.push_back(glm::vec3( 3.0, -3.0,  0.0));
	objPositions.push_back(glm::vec3(-3.0, -3.0,  3.0));
	objPositions.push_back(glm::vec3( 0.0, -3.0,  3.0));
	objPositions.push_back(glm::vec3( 3.0, -3.0,  3.0));
	//填充光的信息
	const GLuint NR_LIGHTS = 32;
	srand(13);
	for (GLuint i = 0; i < NR_LIGHTS; i++)
	{
		// Calculate slightly random offsets
		GLfloat xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
		GLfloat yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
		GLfloat zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
		lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
		// Also calculate random color
		GLfloat rColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
		GLfloat gColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
		GLfloat bColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
		lightColors.push_back(glm::vec3(rColor, gColor, bColor));
	}
}

void RenderNormalScene()
{
	//绘制模型
	model->SetScale(glm::vec3(0.25f));
	for (int i = 0; i < objPositions.size(); ++i)
	{
		model->SetPosition(objPositions[i]);
		model->Update(0);
		model->Draw(modelShaderProgram);
	}
}

void SetupGBuffer()
{
	//创建纹理

	//位置
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//法线
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//颜色反射率 和 高光
	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//创建rbo
	glGenRenderbuffers(1, &gRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, gRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIN_WIDTH, WIN_HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	//创建FBO
	glGenFramebuffers(1, &gFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, gFBO);
	//深度组件
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gFBO);
	//颜色组件
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
	GLenum attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	//设置3个颜色缓冲区
	glDrawBuffers(3, attachments);
	auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf_s("Create FBO failed！\n");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DeferredRender()
{
	glUseProgram(deferredShaderProgram);
	//告诉她纹理，其实是各种信息
	glUniform1i(glGetUniformLocation(deferredShaderProgram, "gPosition"), 0);
	glUniform1i(glGetUniformLocation(deferredShaderProgram, "gNormal"), 1);
	glUniform1i(glGetUniformLocation(deferredShaderProgram, "gAlbedoSpec"), 2);
	glUniform3fv(glGetUniformLocation(deferredShaderProgram, "viewPos"), 1, glm::value_ptr(*camera->GetPosition()));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);

	const int counts = 32;
	char name[32] = {};
	for (int i = 0; i < counts; ++i)
	{
		sprintf_s(name, 32, "lights[%d].lightPos", i);
		glUniform3fv(glGetUniformLocation(deferredShaderProgram, name), 1, glm::value_ptr(lightPositions[i]));
		sprintf_s(name, 32, "lights[%d].lightColor", i);
		glUniform3fv(glGetUniformLocation(deferredShaderProgram, name), 1, glm::value_ptr(lightColors[i]));
	}
	RenderQuad();
}

void DrawLightCube()
{
	//拷贝深度缓冲区
	glBindFramebuffer(GL_READ_FRAMEBUFFER, gFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(
		0,0,WIN_WIDTH, WIN_HEIGHT, 
		0, 0, WIN_WIDTH, WIN_HEIGHT, 
		GL_DEPTH_BUFFER_BIT, 
		GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDisable(GL_CULL_FACE);
	static CMyCube * cube = nullptr;
	if (!cube)
	{
		cube = new CMyCube();
		cube->SetScale(glm::vec3(0.15f));
		cube->SetCamera(camera);
	}
	glUseProgram(lightCubeShaderProgram); 
	for (int i = 0; i < lightPositions.size(); ++i)
	{
		glUniform3fv(glGetUniformLocation(lightCubeShaderProgram, "lightColor"), 1, glm::value_ptr(lightColors[i]));
		cube->SetPosition(lightPositions[i]);
		cube->Update(0.0f);
		cube->Draw(lightCubeShaderProgram);
	}
	glEnable(GL_CULL_FACE);
}
