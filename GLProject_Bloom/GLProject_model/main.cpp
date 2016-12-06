
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

//***************************************

//绘制普通场景
GLuint cubeShaderProgram = 0;
GLuint lightShaderProgram = 0;
GLuint texWood = 0;
GLuint texContainer = 0;
void RenderCube();
void RenderNormalScene();

//创建fbo
GLuint FBO;
//存普通画面
GLuint texNormal;
//存高亮画面
GLuint texLight;
GLuint RBO;
GLuint normalShaderProgram = 0;
GLfloat blurRange = 1.0f;
GLint blurIntensity = 5;
void SetupFBO();
void RenderTexInFBO(GLuint texture, GLuint shaderProgram);

//虚化纹理
GLuint blurShaderProgram = 0;
void BlurTexture(GLuint texture, GLint intensity, GLfloat range);

//泛光
GLuint bloomShaderProgram = 0;
GLfloat bloomExposure = 1.0f;
GLboolean useBloom = 0;
void BloomLight();
//***************************************

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
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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
	window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "OpenGL_model", nullptr, nullptr);
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
	else if (_key == GLFW_KEY_UP && _action == GLFW_RELEASE)
	{
		bloomExposure += 0.1f;
		printf_s("bloom exposure %.2f\n", bloomExposure);
	}
	else if (_key == GLFW_KEY_DOWN && _action == GLFW_RELEASE)
	{
		bloomExposure -= 0.1f;
		printf_s("bloom exposure %.2f\n", bloomExposure);
	}
	//0控制是否开启泛光
	else if (_key == GLFW_KEY_0 && _action == GLFW_RELEASE)
	{
		if (useBloom)
		{
			useBloom = 0;
			printf_s("stop bloom\n");
		}
		else
		{
			useBloom = 1;
			printf_s("use bloom\n");
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
	//绘制通常场景
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	RenderNormalScene();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//到此两张纹理已经分别保存了普通场景 和 高亮部分
	//对高亮部分做虚化
	BlurTexture(texLight, blurIntensity, blurRange);
	//绘制一下虚化后的纹理检测一下
	//glClear(GL_COLOR_BUFFER_BIT);
	//RenderTexInFBO(texLight, normalShaderProgram);

	//泛光
	BloomLight();
}

void InitObject()
{
	//初始化相机
	camera = new CMyCamera(45.0f, WIN_WIDTH, WIN_HEIGHT, glm::vec3(4.0f, 1.0f, 2.5f), glm::vec3(3.1f, 0.8f, 2.1f));
	cubeShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/box.vert", "../bin/shaders/box.frag");
	lightShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/light.vert", "../bin/shaders/light.frag");
	normalShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/normal.vert", "../bin/shaders/normal.frag"); 
	blurShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/blur.vert", "../bin/shaders/blur.frag"); 
	bloomShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/bloom.vert", "../bin/shaders/bloom.frag");
	texWood = LoadTexture("../bin/res/wood.png", true);
	texContainer = LoadTexture("../bin/res/container2.png", true);
	SetupFBO();
}

void RenderNormalScene()
{
	//灯
	vector<glm::vec3> lightPos;
	lightPos.emplace_back(0.0f, 0.5f, 1.5f); // back light
	lightPos.emplace_back(-4.0f, 0.5f, -3.0f);
	lightPos.emplace_back(3.0f, 0.5f, 1.0f);
	lightPos.emplace_back(-.8f, 2.4f, -1.0f);

	vector<glm::vec3> lightColor;
	lightColor.emplace_back(2.0f, 2.0f, 2.0f);
	lightColor.emplace_back(1.5f, 0.0f, 0.0f);
	lightColor.emplace_back(0.0f, 0.0f, 1.5f);
	lightColor.emplace_back(0.0f, 1.5f, 0.0f);

	glUseProgram(cubeShaderProgram);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(cubeShaderProgram, "texDiffuse"), 0);
	glUniformMatrix4fv(glGetUniformLocation(cubeShaderProgram, "matView"), 1, GL_FALSE, glm::value_ptr(*camera->GetView()));
	glUniformMatrix4fv(glGetUniformLocation(cubeShaderProgram, "matProjection"), 1, GL_FALSE, glm::value_ptr(*camera->GetProjection()));
	glUniform3fv(glGetUniformLocation(cubeShaderProgram, "viewPos"), 1, glm::value_ptr(*camera->GetPosition()));
	//设置灯
	char name[32] = {};
	for (int i = 0; i < lightPos.size(); ++i)
	{
		sprintf_s(name, 32, "lights[%d].lightPos", i);
		glUniform3fv(glGetUniformLocation(cubeShaderProgram, name), 1, glm::value_ptr(lightPos[i]));
		sprintf_s(name, 32, "lights[%d].lightColor", i);
		glUniform3fv(glGetUniformLocation(cubeShaderProgram, name), 1, glm::value_ptr(lightColor[i]));
	}
	static glm::mat4 matModel;
	//绘制一个巨大的盒子当地板
	matModel = glm::mat4();
	matModel = glm::translate(matModel, glm::vec3(0.0f, -1.0f, 0.0));
	matModel = glm::scale(matModel, glm::vec3(25.0f, 1.0f, 25.0f));
	glUniformMatrix4fv(glGetUniformLocation(cubeShaderProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(matModel));
	glBindTexture(GL_TEXTURE_2D, texWood);
	RenderCube();

	//绘制小箱子
	glBindTexture(GL_TEXTURE_2D, texContainer);
	matModel = glm::mat4();
	matModel = glm::translate(matModel, glm::vec3(0.0f, 1.5f, 0.0));
	glUniformMatrix4fv(glGetUniformLocation(cubeShaderProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(matModel));
	RenderCube();

	matModel = glm::mat4();
	matModel = glm::translate(matModel, glm::vec3(2.0f, 0.0f, 1.0));
	glUniformMatrix4fv(glGetUniformLocation(cubeShaderProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(matModel));
	RenderCube();

	matModel = glm::mat4();
	matModel = glm::translate(matModel, glm::vec3(-1.0f, -1.0f, 2.0));
	matModel = glm::rotate(matModel, 60.0f, glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	matModel = glm::scale(matModel, glm::vec3(2.0));
	glUniformMatrix4fv(glGetUniformLocation(cubeShaderProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(matModel));
	RenderCube();

	matModel = glm::mat4();
	matModel = glm::translate(matModel, glm::vec3(0.0f, 2.7f, 4.0));
	matModel = glm::rotate(matModel, 23.0f, glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	matModel = glm::scale(matModel, glm::vec3(2.5));
	glUniformMatrix4fv(glGetUniformLocation(cubeShaderProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(matModel));
	RenderCube();

	matModel = glm::mat4();
	matModel = glm::translate(matModel, glm::vec3(-2.0f, 1.0f, -3.0));
	matModel = glm::rotate(matModel, 124.0f, glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	matModel = glm::scale(matModel, glm::vec3(2.0));
	glUniformMatrix4fv(glGetUniformLocation(cubeShaderProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(matModel));
	RenderCube();

	matModel = glm::mat4();
	matModel = glm::translate(matModel, glm::vec3(-3.0f, 0.0f, 0.0));
	glUniformMatrix4fv(glGetUniformLocation(cubeShaderProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(matModel));
	RenderCube();

	//绘制发光物体

	glUseProgram(lightShaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(lightShaderProgram, "matView"), 1, GL_FALSE, glm::value_ptr(*camera->GetView()));
	glUniformMatrix4fv(glGetUniformLocation(lightShaderProgram, "matProjection"), 1, GL_FALSE, glm::value_ptr(*camera->GetProjection()));
	for (int i = 0; i < lightPos.size(); ++i)
	{
		matModel = glm::mat4();
		matModel = glm::translate(matModel, lightPos[i]);
		matModel = glm::scale(matModel, glm::vec3(0.5f));
		glUniformMatrix4fv(glGetUniformLocation(lightShaderProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(matModel));
		glUniform3fv(glGetUniformLocation(lightShaderProgram, "lightColor"), 1, glm::value_ptr(lightColor[i]));
		RenderCube();
	}
}

void RenderCube()
{
	static GLuint cubeVAO = 0;
	static GLuint cubeVBO = 0;
	if (!cubeVAO)
	{
		GLfloat vertices[] = {
			// Back face
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
			0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
			0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,  // top-right
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,  // bottom-left
			-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,// top-left
			// Front face
			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
			0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
			0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,  // top-right
			0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
			-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom-left
			// Left face
			-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
			-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
			-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-left
			-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
			-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
			-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
			// Right face
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
			0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
			0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right         
			0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-right
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // top-left
			0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left     
			// Bottom face
			-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
			0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
			0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,// bottom-left
			0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
			-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
			-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
			// Top face
			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
			0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
			0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right     
			0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
			-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f // bottom-left        
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// Fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// Link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// Render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void SetupFBO()
{
	GLuint textures[2] = { };
	glGenTextures(2, textures);
	texNormal = textures[0];
	texLight = textures[1];
	for (int i = 0; i < 2; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		//HDR
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGB, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}

	//setup rbo
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIN_WIDTH, WIN_HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	//绑定color
	for (int i = 0; i < 2; ++i)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textures[i], 0);
	}
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);
	//设置 multiple render targets
	GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
	auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf_s("Create FBO failed!\n");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTexInFBO(GLuint texture, GLuint shaderProgram)
{
	static GLuint quadVAO = 0;
	static GLuint quadVBO;
	if (!quadVAO)
	{
		GLfloat quadVertices[] = {
			// Positions        // Texture Coords
			-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
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
	glUseProgram(shaderProgram);
	glUniform1i(glGetUniformLocation(shaderProgram, "texFBO"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void BlurTexture(GLuint texture, GLint _intensity, GLfloat _range)
{
	//创建一个quad
	static GLuint quadVAO = 0;
	static GLuint quadVBO;
	if (!quadVAO)
	{
		GLfloat quadVertices[] = {
			// Positions        // Texture Coords
			-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
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

	static bool fin = false;
	static GLuint blurFBO[2] = {};
	static GLuint texTempBlur = 0;
	if (!fin)
	{
		//创建两个fbo
		glGenTextures(1, &texTempBlur);
		glBindTexture(GL_TEXTURE_2D, texTempBlur);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGB, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		glGenFramebuffers(2, blurFBO);
		for (int i = 0; i < 2; ++i)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, blurFBO[i]);
			if (i == 0)
			{
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texTempBlur, 0);
			}
			else
			{
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
			}
			auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if (status != GL_FRAMEBUFFER_COMPLETE)
			{
				printf_s("Create fbo failed\n");
			}
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		fin = true;
	}
	GLfloat range = _range > 0 ? _range : 1.0f;
	GLint intensity = _intensity > 0 ? _intensity : 1;
	glUseProgram(blurShaderProgram);
	glActiveTexture(GL_TEXTURE0);
	//设置虚化次数
	glUniform1f(glGetUniformLocation(blurShaderProgram, "range"), range);
	glUniform1i(glGetUniformLocation(blurShaderProgram, "texSource"), 0);	
	for (int i = 0; i < _intensity; ++i)
	{
		//横向虚化
		glUniform1i(glGetUniformLocation(blurShaderProgram, "horizontal"), 1);
		glBindTexture(GL_TEXTURE_2D, texture);
		glBindFramebuffer(GL_FRAMEBUFFER, blurFBO[0]);
		glClear(GL_COLOR_BUFFER_BIT);  
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//纵向虚化
		glUniform1i(glGetUniformLocation(blurShaderProgram, "horizontal"), 0);
		glBindTexture(GL_TEXTURE_2D, texTempBlur);
		glBindFramebuffer(GL_FRAMEBUFFER, blurFBO[1]);
		glClear(GL_COLOR_BUFFER_BIT);
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	//到此已绘制回原有的texture
}

void BloomLight()
{
	glClear(GL_COLOR_BUFFER_BIT);
	static GLuint quadVAO = 0;
	static GLuint quadVBO;
	if (!quadVAO)
	{
		GLfloat quadVertices[] = {
			// Positions        // Texture Coords
			-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
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
	glUseProgram(bloomShaderProgram);
	glUniform1i(glGetUniformLocation(bloomShaderProgram, "texNormal"), 0);
	glUniform1i(glGetUniformLocation(bloomShaderProgram, "texAffterBlur"), 1);
	glUniform1f(glGetUniformLocation(bloomShaderProgram, "exposure"), bloomExposure);
	glUniform1i(glGetUniformLocation(bloomShaderProgram, "bloom"), useBloom);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texNormal);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texLight);
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}