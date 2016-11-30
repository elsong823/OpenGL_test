
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

//*************************************
GLuint UBO;
void CreateUBO();

GLuint planeVAO;
GLuint planeVBO;
GLuint texPlane;
GLuint planeShaderProgram;
void CreatePlane();
void DrawPlane(GLuint _shaderProgram);

void CreateBox();
void UpdateBox(GLfloat _dt);
void DrawBox(GLuint _shaderProgram);

GLuint boxShaderProgram;
vector<CMyBox*> boxes;

D_LIGHT dlight;
void CreateLight();

GLuint depthMapFBO;
GLuint texDepthMap;
GLuint depthShaderProgram;
const GLint SHADOW_WIDTH = 1024*2;
const GLint SHADOW_HEIGHT = 1024*2;
void CreateDepthMap();


GLuint quadVAO = 0;
GLuint quadVBO;
GLuint quadShaderProgram = 0;
glm::mat4 lightMatSpace;
void RenderQuad();

GLuint lightCube;
void DrawLight();
//*************************************
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
	//glm::mat4 matRotate;
	//matRotate = glm::rotate(matRotate, _dt, glm::vec3(0.0f,0.0f,1.0f));
	//dlight.direction = glm::vec3(matRotate * glm::vec4(dlight.direction, 0.0f));
	CheckKeyStatus();
	camera->Update(_dt);
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 16 * 4, glm::value_ptr(*camera->GetView()));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	UpdateBox(_dt);
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
	//render depth of scene to texture from light perspective
	//Get light projection and view matrix
	static glm::mat4 lightMatView;
	static glm::mat4 lightMatProjection;
	GLfloat nearPlane = 1.0f;
	GLfloat farPlane = 15.0f;
	lightMatProjection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, nearPlane, farPlane);
	lightMatView = glm::lookAt(-dlight.direction, glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	lightMatSpace = lightMatProjection * lightMatView;
	//Render scene from light's point of view
	glUseProgram(depthShaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(depthShaderProgram, "matLightSpace"), 1, GL_FALSE, glm::value_ptr(lightMatSpace));
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	DrawPlane(depthShaderProgram);
	DrawBox(depthShaderProgram);
	glCullFace(GL_BACK);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//reset
	glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//RenderQuad();
	DrawPlane(planeShaderProgram);
	DrawBox(boxShaderProgram);
	DrawLight();

	glBindTexture(GL_TEXTURE_2D, 0);
}

void InitObject()
{
	CreateUBO();
	//初始化相机
	camera = new CMyCamera(45.0f, WIN_WIDTH, WIN_HEIGHT, glm::vec3(0.0f, 1.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	//先绑定一个投影矩阵
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 16 * 4, 16 * 4, glm::value_ptr(*camera->GetProjection()));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	CreatePlane();
	CreateBox();
	CreateLight();
	CreateDepthMap();
}

void CreateUBO()
{
	glGenBuffers(1, &UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4)*2, nullptr, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void CreatePlane()
{
	GLfloat planeVertices[] = {
		// Positions            // Texture Coords (note we set these higher than 1 that together with GL_REPEAT as texture wrapping mode will cause the floor texture to repeat)
		-25.0f, -1.5f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 2.0f,
		-25.0f, -1.5f, 25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		25.0f, -1.5f, 25.0f, 0.0f, 1.0f, 0.0f, 2.0f, 0.0f,

		-25.0f, -1.5f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 2.0f,
		25.0f, -1.5f, 25.0f, 0.0f, 1.0f, 0.0f, 2.0f, 0.0f,
		25.0f, -1.5f, -25.0f, 0.0f, 1.0f, 0.0f, 2.0f, 2.0f
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

	planeShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/plane.vert", "../bin/shaders/plane.frag");
	auto idx = glGetUniformBlockIndex(planeShaderProgram, "Matrices");
	glUniformBlockBinding(planeShaderProgram, idx, 1);

	texPlane = LoadTexture("../bin/res/plane.jpg");
}

void DrawPlane(GLuint _shaderProgram)
{
	static glm::mat4 matModel;
	glUseProgram(_shaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(_shaderProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(matModel));
	glUniformMatrix4fv(glGetUniformLocation(_shaderProgram, "matLightSpace"), 1, GL_FALSE, glm::value_ptr(lightMatSpace));
	glUniform3fv(glGetUniformLocation(_shaderProgram, "viewPos"), 1, glm::value_ptr(*camera->GetPosition()));
	glUniform3fv(glGetUniformLocation(_shaderProgram, "fragLightDir"), 1, glm::value_ptr(dlight.direction));
	glUniform1i(glGetUniformLocation(_shaderProgram, "texDiff"), 0);
	glUniform1i(glGetUniformLocation(_shaderProgram, "texShadow"), 1);
	glBindVertexArray(planeVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texPlane);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texDepthMap);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
}

void CreateBox()
{
	vector<glm::vec3> boxPos;
	boxPos.emplace_back(0.0f, 1.5f, 0.0f);
	boxPos.emplace_back(2.0f, 0.0f, 1.0);
	boxPos.emplace_back(-1.0f, 0.0f, 2.0);

	vector<glm::vec3> boxRotate;
	boxRotate.emplace_back(1.0, 0.0, 1.0);
	boxRotate.emplace_back(1.0, 0.0, 1.0);
	boxRotate.emplace_back(1.0, 0.0, 1.0);
	srand(glfwGetTime());
	for (int i = 0; i < boxPos.size(); ++i)
	{
		CMyBox * box = new CMyBox();
		box->SetPosition(boxPos[i]);
		if (i == 2)
		{
			box->SetRotation(boxRotate[i], 60);
			box->SetScale(glm::vec3(0.5f));
		}

		boxes.emplace_back(box);
	}
	boxShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/box.vert", "../bin/shaders/box.frag");
	auto idx = glGetUniformBlockIndex(boxShaderProgram, "Matrices");
	glUniformBlockBinding(boxShaderProgram, idx, 1);
}

void UpdateBox(GLfloat _dt)
{
	//static GLfloat timer = 0.0f;
	//timer += _dt * 15;
	//static glm::vec3 rotateAxis[3];
	//static bool init = false;
	//if (!init)
	//{
	//	init = true;
	//	for (int i = 0; i < 3; ++i)
	//	{
	//		float x = rand() % 15;
	//		float y = rand() % 15;
	//		float z = rand() % 15;
	//		rotateAxis[i] = glm::vec3(x, y, z);
	//	}
	//}
	for (int i = 0; i < 3; ++i)
	{

		//boxes[i]->SetRotation(rotateAxis[i], timer);
		boxes[i]->Update(_dt);
	}
}

void DrawBox(GLuint _shaderProgram)
{	
	glUseProgram(_shaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(_shaderProgram, "matLightSpace"), 1, GL_FALSE, glm::value_ptr(lightMatSpace));
	glUniform3fv(glGetUniformLocation(_shaderProgram, "viewPos"), 1, glm::value_ptr(*camera->GetPosition()));
	glUniform3fv(glGetUniformLocation(_shaderProgram, "fragLightDir"), 1, glm::value_ptr(dlight.direction));
	for (auto box : boxes)
	{
		box->Draw(_shaderProgram, texDepthMap);
	}
}

void CreateLight()
{
	dlight.direction = glm::vec3(2.0f, -4.0f,  1.0f);
	CMyBox::SetDirectLight(dlight);
}

void CreateDepthMap()
{
	//gl fbo
	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &texDepthMap);

	//gl bind fbo
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	//gl gen tex depth
	glBindTexture(GL_TEXTURE_2D, texDepthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
	//bind tex to fbo
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texDepthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf_s("Create fbo failed!\n");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//creaet shader
	depthShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/depth.vert","../bin/shaders/depth.frag");
}

void RenderQuad()
{
	if (quadVAO == 0)
	{
		quadShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/quad.vert","../bin/shaders/quad.frag");
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
	glUseProgram(quadShaderProgram);
	glBindVertexArray(quadVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texDepthMap);
	glUniform1i(glGetUniformLocation(quadShaderProgram, "texDiff"), 0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
}

void DrawLight()
{
	static GLuint VAO;
	static GLuint VBO;
	static GLuint lightLineShaderProgram;
	if (!VAO)
	{
		float vertices[] = {
			0.0f, 0.0f, 0.0f
		};
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 3, 0);
		glBindVertexArray(0);
		lightLineShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/lightLine.vert","../bin/shaders/lightLine.geo","../bin/shaders/lightLine.frag");
		auto idx = glGetUniformBlockIndex(lightLineShaderProgram, "Matrices");
		glUniformBlockBinding(lightLineShaderProgram, idx, 1);
	} 
	glUseProgram(lightLineShaderProgram);
	glUniform3fv(glGetUniformLocation(lightLineShaderProgram, "lightPos"), 1, glm::value_ptr(-dlight.direction));
	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, 1);
	glBindVertexArray(0);
}