
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

//**************************************

const int SHADOW_WIDTH = 1024;
const int SHADOW_HEIGHT = 1024;
glm::mat4 matLight;
glm::vec3 dirLight;
GLuint shadowFBO = 0;
GLuint texShadow = 0;
GLuint shadowShaderProgram = 0;

GLuint UBO;
void CreateUBO();
void CreateDirectLight();
void CreateDepthMap();
GLuint normalShaderProgram;
void DrawPlane(GLuint _shaderProgram);

GLuint boxVAO = 0;
GLuint boxVBO = 0;
GLuint texBoxDiff = 0;
GLuint texBoxSpec = 0;
GLuint boxShaderProgram = 0;
GLuint newBoxShaderProgram = 0;	
glm::mat4 matrices[5] = {};
GLuint matBuffer = 0;
void CreateBox();
void DrawBox(GLuint _shaderProgram);
void UpdateBox(GLfloat _dt);

void DrawDirectLine();
void DrawShadowMap();

//点光源位置
glm::vec3 ptLightPos(0.0f, 0.0f, 0.0f);
GLuint houseShaderProgram = 0;
void DrawHouse(GLuint _shadeProgram);
//点光源阴影
GLuint plShadowFBO = 0;
GLuint texPLshadow = 0;
GLuint plShadowShaderProgram = 0;
const GLfloat FAR_PLANE = 25.0f;
void CreatePLShadowMap();
//每一面的光空间矩阵
vector<glm::mat4> matLightMatrices;
void SetupLightMatrices();
void SetLightMatrices(GLuint _shaderProgram);
//绘制点光源阴影深度图
void RenderPLShadowTest();
//绘制表示点光源的方形
void RenderLightPoint();
//测试 读取天空盒
GLuint texSkyBox;
void LoadTestSkyBox();

//**************************************

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
	else if (_key == GLFW_KEY_LEFT && _action == GLFW_RELEASE)
	{
		dirLight.x += 1.0f;
		printf_s("%.2f, %.2f, %.2f\n", dirLight.x, dirLight.y, dirLight.z);
	}
	else if (_key == GLFW_KEY_RIGHT && _action == GLFW_RELEASE)
	{
		dirLight.x -= 1.0f;
		printf_s("%.2f, %.2f, %.2f\n", dirLight.x, dirLight.y, dirLight.z);
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
	UpdateBox(_dt);
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, glm::value_ptr(*camera->GetView()));
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
	//Draw shadow map
	//glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	//glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	//glClear(GL_DEPTH_BUFFER_BIT);
	//glCullFace(GL_FRONT);
	//DrawPlane(shadowShaderProgram);
	//DrawBox(shadowShaderProgram);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);
	//glCullFace(GL_BACK);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	////DrawShadowMap();
	//DrawPlane(normalShaderProgram);
	//DrawBox(boxShaderProgram);
	//DrawDirectLine();
	
	//绘制点光源阴影
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, plShadowFBO);
	//重要：一定要先清空深度缓冲区
	glClear(GL_DEPTH_BUFFER_BIT);
	DrawHouse(plShadowShaderProgram);
	DrawBox(plShadowShaderProgram);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	DrawHouse(houseShaderProgram);
	DrawBox(newBoxShaderProgram);
	RenderLightPoint();

}

void InitObject()
{
	//Create ubo
	CreateUBO();
	//LoadTestSkyBox();
	//CreateShader
	normalShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/normal.vert", "../bin/shaders/normal.frag");
	auto idx = glGetUniformBlockIndex(normalShaderProgram, "Matrices");
	glUniformBlockBinding(normalShaderProgram, idx, 1);

	//初始化相机XXX
	camera = new CMyCamera(glm::radians(45.0f), WIN_WIDTH, WIN_HEIGHT, glm::vec3(0.0f, 0.0f, 0.1f), glm::vec3(-1.0f, 0.0f, 0.0f));
	//camera = new CMyCamera(90.0f, SHADOW_WIDTH, SHADOW_HEIGHT, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f, 0.0f));
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, glm::value_ptr(*camera->GetProjection()));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	//CreateDirectLight();
	//CreateDepthMap();
	CreateBox();
	CreatePLShadowMap();
}

void CreateDirectLight()
{
	dirLight = glm::normalize(glm::vec3(-3.0f, -1.0f, 0.0f));
}

void CreateDepthMap()
{
	//gl fbo
	glGenFramebuffers(1, &shadowFBO);
	//gl bind fbo
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	//gl gen tex depth
	glGenTextures(1, &texShadow);
	glBindTexture(GL_TEXTURE_2D, texShadow);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
	//bind tex to fbo
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texShadow, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf_s("Create fbo failed!\n");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	shadowShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/depth.vert", "../bin/shaders/depth.frag");
	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 30.0f);
	glm::mat4 lightView = glm::lookAt(-dirLight * 15.0f, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	matLight = lightProjection * lightView;
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 64 * 2, 64, glm::value_ptr(matLight));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void DrawPlane(GLuint _shaderProgram)
{
	static GLuint planeVAO = 0;
	static GLuint planeVBO = 0;
	static GLuint texPlane = 0;
	if (!planeVAO)
	{
		GLfloat planeVertices[] = {
			-25.0f, -1.5f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 2.0f,
			-25.0f, -1.5f, 25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			25.0f, -1.5f, -25.0f, 0.0f, 1.0f, 0.0f, 2.0f, 2.0f,
			25.0f, -1.5f, 25.0f, 0.0f, 1.0f, 0.0f, 2.0f, 0.0f
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
		//Create tex
		texPlane = LoadTexture("../bin/res/16M58PICi24_1024.jpg");
	}
	static glm::mat4 matModel;
	glUseProgram(_shaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(_shaderProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(matModel));
	glUniform1i(glGetUniformLocation(_shaderProgram, "texDiff"), 0);
	glUniform1i(glGetUniformLocation(_shaderProgram, "texShadow"), 1);
	glUniform1i(glGetUniformLocation(_shaderProgram, "type"), 1);
	glUniform3fv(glGetUniformLocation(_shaderProgram, "dirLight"), 1, glm::value_ptr(dirLight));
	glUniformMatrix4fv(glGetUniformLocation(_shaderProgram, "matLight"), 1, GL_FALSE, glm::value_ptr(matLight));

	glBindVertexArray(planeVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texPlane);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texShadow);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void CreateBox()
{
	GLfloat _vertices[] = {
		// Positions           // Normals           // Texture Coords
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
	glGenVertexArrays(1, &boxVAO);
	glGenBuffers(1, &boxVBO);

	glBindVertexArray(boxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(_vertices), _vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 8, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 8, (GLvoid*)(sizeof(GLfloat)* 3));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 8, (GLvoid*)(sizeof(GLfloat)* 6));

	vector<glm::vec3> pos;
	pos.emplace_back(4.0f, -3.5f, 0.0f);
	pos.emplace_back(2.0f, -1.5f, 0.0f);
	pos.emplace_back(-3.0f, -1.0f, 0.0f);
	pos.emplace_back(-1.5f, 1.0f, 1.5f);
	pos.emplace_back(-1.5f, 2.0f, -3.0f);

	vector<glm::vec3> rotate;
	rotate.emplace_back(0.0f, 0.0f, 2.0f);
	rotate.emplace_back(1.0f, 0.5f, 3.0f);
	rotate.emplace_back(2.0f, 1.0f, 4.0f);
	rotate.emplace_back(3.0f, 1.5f, 5.0f);
	rotate.emplace_back(4.0f, 2.0f, 6.0f);	
	
	vector<glm::vec3> scale;
	scale.emplace_back(1.0f, 1.0f, 1.0f);
	scale.emplace_back(0.3f, 0.3f, 0.3f);
	scale.emplace_back(1.0f, 1.0f, 1.0f);
	scale.emplace_back(1.0f, 1.0f, 1.0f);
	scale.emplace_back(1.0f, 1.0f, 1.0f);


	for (int i = 0; i < pos.size(); ++i)
	{
		matrices[i] = glm::mat4();
		matrices[i] = glm::translate(matrices[i], pos[i]);
		matrices[i] = glm::rotate(matrices[i], glm::radians(rand() % 360 * 1.0f), rotate[i]);
		matrices[i] = glm::scale(matrices[i], scale[i]);
	}

	glGenBuffers(1, &matBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, matBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(matrices), matrices, GL_STATIC_DRAW);
	auto size = sizeof(glm::vec4);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), 0);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(size));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(size * 2));
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(size * 3));
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);

	glBindVertexArray(0);

	//Create tex
	texBoxDiff = LoadTexture("../bin/res/container2.png");
	texBoxSpec = LoadTexture("../bin/res/container2_specular.png");

	//Create shader
	boxShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/box.vert", "../bin/shaders/box.frag");
	auto idx = glGetUniformBlockIndex(boxShaderProgram, "Matrices");
	glUniformBlockBinding(boxShaderProgram, idx, 1);
	//new shader
	newBoxShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/newBox.vert", "../bin/shaders/newBox.frag");
	idx = glGetUniformBlockIndex(boxShaderProgram, "Matrices");
	glUniformBlockBinding(boxShaderProgram, idx, 1);
	
}

void DrawBox(GLuint _shaderProgram)
{
	glUseProgram(_shaderProgram);
	glUniform1i(glGetUniformLocation(_shaderProgram, "texDiff"), 0);
	glUniform1i(glGetUniformLocation(_shaderProgram, "texSpec"), 1);
	glUniform1i(glGetUniformLocation(_shaderProgram, "texShadow"), 2);
	glUniform1i(glGetUniformLocation(_shaderProgram, "type"), 2);
	glUniform1f(glGetUniformLocation(_shaderProgram, "farPlane"), FAR_PLANE);
	glUniform3fv(glGetUniformLocation(_shaderProgram, "dirLight"), 1, glm::value_ptr(dirLight));
	glUniform3fv(glGetUniformLocation(_shaderProgram, "viewPos"), 1, glm::value_ptr(*camera->GetPosition()));
	glUniform3fv(glGetUniformLocation(_shaderProgram, "lightPos"), 1, glm::value_ptr(ptLightPos));
	SetLightMatrices(_shaderProgram);
	//glUniformMatrix4fv(glGetUniformLocation(_shaderProgram, "matLight"), 1, GL_FALSE, glm::value_ptr(matLight));
	//glUniform1i(glGetUniformLocation(_shaderProgram, "texShadow"), 2);
	//glUniform1i(glGetUniformLocation(_shaderProgram, "type"), 2);
	glBindVertexArray(boxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texBoxDiff);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texBoxSpec);
	glActiveTexture(GL_TEXTURE2);
	//glBindTexture(GL_TEXTURE_2D, texShadow);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texPLshadow);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 36, 5);

	glBindVertexArray(0);
}

void CreateUBO()
{
	glGenBuffers(1, &UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 3, nullptr, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void DrawDirectLine()
{
	static GLuint VAO;
	static GLuint VBO;
	static GLuint dLightShaderProgram;
	if (!VAO)
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* 3, nullptr, GL_STATIC_DRAW);
		glBindVertexArray(0);
		dLightShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/dLight.vert", "../bin/shaders/dLight.geo", "../bin/shaders/dLight.frag");
		auto idx = glGetUniformBlockIndex(dLightShaderProgram, "Matrices");
		glUniformBlockBinding(dLightShaderProgram, idx, 1);
	}
	glUseProgram(dLightShaderProgram);
	glUniform3fv(glGetUniformLocation(dLightShaderProgram, "dirLight"), 1, glm::value_ptr(dirLight));
	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, 1);
	glBindVertexArray(0);
}

void DrawShadowMap()
{
	static GLuint VAO;
	static GLuint VBO;
	static GLuint shaderProgram;
	if (!VAO)
	{
		GLfloat planeVertices[] = {
			-1.0f,  1.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 1.0f, 0.0f
		};
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 4, 0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 4, (GLvoid*)(sizeof(GLfloat)* 2));
		glBindVertexArray(0);
		shaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/shadowShow.vert", "../bin/shaders/shadowShow.frag");
	}
	static glm::mat4 matModel;
	glUseProgram(shaderProgram);

	glUniform1i(glGetUniformLocation(shaderProgram, "texShadow"), 0);
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texShadow);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	glBindVertexArray(0);
}

//画一个房子
void DrawHouse(GLuint _shadeProgram)
{
	static GLuint houseVAO = 0;
	static GLuint houseVBO = 0;
	static GLuint texHouse = 0;
	if (!houseVAO)
	{
		GLfloat _vertices[] = {
			// Positions           // Normals           // Texture Coords
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f, // Bottom-left
			0.5f, 0.5f, -0.5f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f, // top-right
			0.5f, -0.5f, -0.5f, 0.0f, 0.0f,  1.0f, 1.0f, 0.0f, // bottom-right         
			0.5f, 0.5f, -0.5f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f,  // top-right
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom-left
			-0.5f, 0.5f, -0.5f, 0.0f, 0.0f,  1.0f, 0.0f, 1.0f,// top-left
			// Front face
			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			0.5f, -0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,  // bottom-right
			0.5f, 0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,  // top-right
			0.5f, 0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,  // top-left
			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,  // bottom-left
			// Left face
			-0.5f, 0.5f, 0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
			-0.5f, 0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
			-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-left
			-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
			-0.5f, -0.5f, 0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
			-0.5f, 0.5f, 0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
			// Right face
			0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
			0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
			0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right         
			0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-right
			0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // top-left
			0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left     
			// Bottom face
			-0.5f, -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f, // top-right
			0.5f, -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 1.0f, 1.0f, // top-left
			0.5f, -0.5f, 0.5f, 0.0f,  1.0f, 0.0f, 1.0f, 0.0f,// bottom-left
			0.5f, -0.5f, 0.5f, 0.0f,  1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
			-0.5f, -0.5f, 0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
			-0.5f, -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f, // top-right
			// Top face
			-0.5f, 0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,// top-left
			0.5f, 0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
			0.5f, 0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-right     
			0.5f, 0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
			-0.5f, 0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,// top-left
			-0.5f, 0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f // bottom-left   
		};

		glGenVertexArrays(1, &houseVAO);
		glGenBuffers(1, &houseVBO);

		glBindVertexArray(houseVAO);
		glBindBuffer(GL_ARRAY_BUFFER, houseVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(_vertices), _vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 8, 0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 8, (GLvoid*)(sizeof(GLfloat)* 3));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 8, (GLvoid*)(sizeof(GLfloat)* 6));
		glBindVertexArray(0);

		//tex
		texHouse = LoadTexture("../bin/res/240448-1510240IK660.jpg");

		houseShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/house.vert", "../bin/shaders/house.frag");
		auto idx = glGetUniformBlockIndex(houseShaderProgram, "Matrices");
		glUniformBlockBinding(houseShaderProgram, idx, 1);
	}

	glUseProgram(_shadeProgram);
	glm::mat4 matModel;
	matModel = glm::scale(matModel, glm::vec3(10.0f));
	glUniformMatrix4fv(glGetUniformLocation(_shadeProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(matModel));
	glUniform1i(glGetUniformLocation(_shadeProgram, "texWall"), 0);
	glUniform1i(glGetUniformLocation(_shadeProgram, "texShadow"), 1);
	glUniform1i(glGetUniformLocation(_shadeProgram, "type"), 1);

	SetLightMatrices(_shadeProgram);
	glBindVertexArray(houseVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texHouse);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texPLshadow);
	glCullFace(GL_FRONT);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glCullFace(GL_BACK);
	glBindVertexArray(0);
}

void CreatePLShadowMap()
{
	//生成纹理
	glGenTextures(1, &texPLshadow);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texPLshadow);
	for (int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	}
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glGenFramebuffers(1, &plShadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, plShadowFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texPLshadow, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf_s("Create fbo failed!\n");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	//Create shader
	plShadowShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/plDepthRender.vert", "../bin/shaders/plDepthRender.geo", "../bin/shaders/plDepthRender.frag");
}

void RenderPLShadowTest()
{

}

void RenderLightPoint()
{
	static GLuint VAO;
	static GLuint VBO;
	static GLuint colorShader;
	if (!VAO)
	{
		GLfloat _vertices[] = {
			// Positions         
			-0.5f, -0.5f, -0.5f, 
			0.5f, 0.5f, -0.5f, 
			0.5f, -0.5f, -0.5f,
			0.5f, 0.5f, -0.5f, 
			-0.5f, -0.5f, -0.5f,
			-0.5f, 0.5f, -0.5f,
			// Front face
			-0.5f, -0.5f, 0.5f,
			0.5f, -0.5f, 0.5f, 
			0.5f, 0.5f, 0.5f,
			0.5f, 0.5f, 0.5f,
			-0.5f, 0.5f, 0.5f, 
			-0.5f, -0.5f, 0.5f,
			// Left face
			-0.5f, 0.5f, 0.5f, 
			-0.5f, 0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f, 
			-0.5f, -0.5f, -0.5f, 
			-0.5f, -0.5f, 0.5f,
			-0.5f, 0.5f, 0.5f, 
			// Right face
			0.5f, 0.5f, 0.5f, 
			0.5f, -0.5f, -0.5f,
			0.5f, 0.5f, -0.5f, 
			0.5f, -0.5f, -0.5f,
			0.5f, 0.5f, 0.5f,
			0.5f, -0.5f, 0.5f, 
			// Bottom face
			-0.5f, -0.5f, -0.5f, 
			0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, 0.5f, 
			0.5f, -0.5f, 0.5f, 
			-0.5f, -0.5f, 0.5f,
			-0.5f, -0.5f, -0.5f, 
			// Top face
			-0.5f, 0.5f, -0.5f,
			0.5f, 0.5f, 0.5f, 
			0.5f, 0.5f, -0.5f, 
			0.5f, 0.5f, 0.5f, 
			-0.5f, 0.5f, -0.5f,
			-0.5f, 0.5f, 0.5f, 
		};
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(_vertices), _vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 3, 0);
		colorShader = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/colorCube.vert", "../bin/shaders/colorCube.frag");
		auto idx = glGetUniformBlockIndex(colorShader, "Matrices");
		glUniformBlockBinding(colorShader, idx, 1);
	}
	glUseProgram(colorShader);
	glm::mat4 matModel;
	matModel = glm::translate(matModel, ptLightPos);
	matModel = glm::scale(matModel, glm::vec3(0.1f));
	glUniformMatrix4fv(glGetUniformLocation(colorShader, "matModel"), 1, GL_FALSE, glm::value_ptr(matModel));
	glUniform3f(glGetUniformLocation(colorShader,"inColor"), 1.0f, 1.0f, 0.0f);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void SetupLightMatrices()
{
	glm::mat4 matProjection = glm::perspective(glm::radians(90.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, 0.1f, FAR_PLANE);
	//glm::mat4 matProjection = glm::perspective(90.0f, (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, 1.0f, FAR_PLANE);
	//POSITIVE X
	matLightMatrices.emplace_back(matProjection * glm::lookAt(ptLightPos, ptLightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0,  -1.0, 0.0)));
	//NEGATIVE X
	matLightMatrices.emplace_back(matProjection * glm::lookAt(ptLightPos, ptLightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	////POSITIVE Y
	matLightMatrices.emplace_back(matProjection * glm::lookAt(ptLightPos, ptLightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f,    1.0f)));
	////NEGATIVE Y
	matLightMatrices.emplace_back(matProjection * glm::lookAt(ptLightPos, ptLightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f,  -1.0f)));
	////POSITIVE Z
	matLightMatrices.emplace_back(matProjection * glm::lookAt(ptLightPos, ptLightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	////NEGATIVE Z
	matLightMatrices.emplace_back(matProjection * glm::lookAt(ptLightPos, ptLightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
}

void SetLightMatrices(GLuint _shaderProgram)
{
	if (!matLightMatrices.size())
	{
		SetupLightMatrices();
	}
	glUseProgram(_shaderProgram);
	for (int i = 0; i < matLightMatrices.size(); ++i)
	{
		char name[32] = {};
		sprintf_s(name, 32, "matLightMatrices[%d]", i);
		auto t = glGetUniformLocation(_shaderProgram, name);
		glUniformMatrix4fv(t, 1, GL_FALSE, glm::value_ptr(matLightMatrices[i]));
	}
	//设置光的位置等
	glUniform3fv(glGetUniformLocation(_shaderProgram, "lightPos"), 1, glm::value_ptr(ptLightPos));
	glUniform1f(glGetUniformLocation(_shaderProgram, "farPlane"), FAR_PLANE);	
}

void LoadTestSkyBox()
{
	vector<string> pics;
	pics.emplace_back("../bin/res/sky/right.jpg");
	pics.emplace_back("../bin/res/sky/left.jpg");
	pics.emplace_back("../bin/res/sky/top.jpg");
	pics.emplace_back("../bin/res/sky/bottom.jpg");
	pics.emplace_back("../bin/res/sky/back.jpg");
	pics.emplace_back("../bin/res/sky/front.jpg");

	glGenTextures(1, &texSkyBox);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texSkyBox);
	GLubyte * source = nullptr;
	GLint width, height;
	for (int i = 0; i < pics.size(); ++i)
	{
		source = SOIL_load_image(pics[i].c_str(), &width, &height, nullptr, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, source);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void UpdateBox(GLfloat _dt)
{
	vector<glm::vec3> pos;
	pos.emplace_back(4.0f, -3.5f, 0.0f);
	pos.emplace_back(2.0f, -1.5f, 0.0f);
	pos.emplace_back(-3.0f, -1.0f, 0.0f);
	pos.emplace_back(-1.5f, 1.0f, 1.5f);
	pos.emplace_back(-1.5f, 2.0f, -3.0f);

	vector<glm::vec3> rotate;
	rotate.emplace_back(0.0f, 0.0f, 2.0f);
	rotate.emplace_back(1.0f, 0.5f, 3.0f);
	rotate.emplace_back(2.0f, 1.0f, 4.0f);
	rotate.emplace_back(3.0f, 1.5f, 5.0f);
	rotate.emplace_back(4.0f, 2.0f, 6.0f);

	vector<glm::vec3> scale;
	scale.emplace_back(1.0f, 1.0f, 1.0f);
	scale.emplace_back(0.3f, 0.3f, 0.3f);
	scale.emplace_back(1.0f, 1.0f, 1.0f);
	scale.emplace_back(1.0f, 1.0f, 1.0f);
	scale.emplace_back(1.0f, 1.0f, 1.0f);

	static float _rotation = 0.0f;
	_rotation += _dt * 10.0f;
	for (int i = 0; i < pos.size(); ++i)
	{
		matrices[i] = glm::mat4();
		matrices[i] = glm::translate(matrices[i], pos[i]);
		matrices[i] = glm::rotate(matrices[i], glm::radians(_rotation), rotate[i]);
		matrices[i] = glm::scale(matrices[i], scale[i]);
	}
	GLfloat _vertices[] = {
		// Positions           // Normals           // Texture Coords
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

	glBindBuffer(GL_ARRAY_BUFFER, matBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(matrices), matrices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}