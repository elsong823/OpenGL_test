
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
GLuint cubeShaderProgram = 0;
GLuint modelShaderProgram = 0;
D_LIGHT directLight;
PT_LIGHT pointLight_1;
PT_LIGHT pointLight_2;
PT_LIGHT pointLight_3;
S_LIGHT spotLight;
CMyCamera * camera = nullptr;
vector<CMyCube *> boxes;
CMyModel * model = nullptr;

GLuint pointsVAO, pointsVBO;
GLuint pointShaderProgram;

GLuint pointsVAO2, pointsVBO2;
GLuint pointShaderProgram2;

GLuint cubeVAO, cubeVBO;
GLuint shaderProgramRed, shaderProgramGreen, shaderProgramBlue, shaderProgramYellow;
GLuint UBO;
GLuint colorShaderProgram;

void Init();
void InitObject();

void InitPoints();
void DrawPoints();

void Exit();
void Draw();
void Update(GLfloat _dt);
void CheckKeyStatus();
void KeyCallback(GLFWwindow * _win, int _key, int _scanCode, int _action, int _mode);
void CursorCallback(GLFWwindow * _win, double _offsetX, double _offsetY);

void BindUniformBuffer();
void CreateCube();
void DrawCubes();

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

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CW);

	//��ʼ������
	InitObject();
	//��uniform buffer
	BindUniformBuffer();
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
	//���ð���
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

	//Update direct light
	//auto matRotate = glm::mat4();
	//matRotate = glm::rotate(matRotate, _dt, glm::vec3(0.0f, 0.0f, 1.0f));
	//directLight.direction = glm::vec3(matRotate * glm::vec4(directLight.direction,0.0f));
	for (auto box : boxes)
	{
		//CMyBox::SetDirectLight(directLight);
		box->Update(_dt);
	}
	if (model)
	{
		model->Update(_dt);
	}
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
	//DrawPoints();
	//for (auto box : boxes)
	//{
	//	box->Draw(cubeShaderProgram);
	//}
	//if (model)
	//{
	//	model->Draw(modelShaderProgram);
	//}
	DrawCubes();
}

void InitObject()
{
	//��ʼ�����
	camera = new CMyCamera(45.0f, WIN_WIDTH, WIN_HEIGHT, glm::vec3(0.0f, 1.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	//��ʼ��shader
	pointShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/point.vert", "../bin/shaders/point.frag");
	pointShaderProgram2 = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/point2.vert", "../bin/shaders/point2.frag");
	shaderProgramBlue = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/cube.vert", "../bin/shaders/cubeBlue.frag");
	shaderProgramGreen = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/cube.vert", "../bin/shaders/cubeGreen.frag");
	shaderProgramYellow = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/cube.vert", "../bin/shaders/cubeYellow.frag");
	shaderProgramRed = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/cube.vert", "../bin/shaders/cubeRed.frag");
	colorShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/cube.vert", "../bin/shaders/cube.frag");

	//InitPoints();

	////��ʼ���ƹ�
	//directLight.direction = glm::vec3(-1.0f, -1.0f, 0.0f);
	//directLight.ambient = glm::vec3(0.2f);
	//directLight.diffuse = glm::vec3(0.5f);
	//directLight.specular = glm::vec3(0.8f);

	////��ʼ�����Դ
	//pointLight_1.position = glm::vec3(0.0f, 0.0f, 1.3f);
	//pointLight_1.ambient = glm::vec3(0.1f);
	//pointLight_1.diffuse = glm::vec3(1.0f);
	//pointLight_1.specular = glm::vec3(1.0f);
	//pointLight_1.constant = 1.0f;
	//pointLight_1.linear = 0.7f;
	//pointLight_1.quadratic = 1.8f;
	//
	//pointLight_2.position = glm::vec3(1.7f, 0.0f, -1.3f);
	//pointLight_2.ambient = glm::vec3(0.1f);
	//pointLight_2.diffuse = glm::vec3(1.0);
	//pointLight_2.specular = glm::vec3(1.0f);
	//pointLight_2.constant = 1.0f;
	//pointLight_2.linear = 0.7f;
	//pointLight_2.quadratic = 1.8f;

	//pointLight_3.position = glm::vec3(-1.7f, 0.0f, -1.3f);
	//pointLight_3.ambient = glm::vec3(0.1f);
	//pointLight_3.diffuse = glm::vec3(0.0f, 1.0f, 1.0);
	//pointLight_3.specular = glm::vec3(1.0f);
	//pointLight_3.constant = 1.0f;
	//pointLight_3.linear = 0.7f;
	//pointLight_3.quadratic = 1.8f;

	////��ʼ���۹��
	//spotLight.ambient = glm::vec3(0.1f);
	//spotLight.diffuse = glm::vec3(1.0f);
	//spotLight.specular = glm::vec3(1.0f);
	//spotLight.cutoff = cos(glm::radians(10.0f));
	//spotLight.outCutoff = cos(glm::radians(15.0f));
	//spotLight.constant = 1.0f;
	//spotLight.linear = 0.22f;
	//spotLight.quadratic = 0.20f;

	//cubeShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/box.vert", "../bin/shaders/box.frag");
	//modelShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/model.vert", "../bin/shaders/model.frag");


	////��ʼ������
	//auto box = new CMyBox();
	//box->SetPosition(glm::vec3(1.0f, 2.2f, 0.4f));
	//box->SetScale(glm::vec3(0.1f));
	////box->SetRotation(glm::vec3(1.0f, 1.0f, 0.0f), 30);
	//box->SetCamera(camera);
	//boxes.push_back(box);
	//box->AddPointLight(pointLight_1);
	//box->AddPointLight(pointLight_2);
	//box->AddPointLight(pointLight_3);

	//box = new CMyBox();
	//box->SetPosition(glm::vec3(1.0f, 5.7f, 0.3f));
	//box->SetScale(glm::vec3(0.1f));
	////box->SetRotation(glm::vec3(1.0f, 1.0f, 0.0f), 30);
	//box->SetCamera(camera);
	//boxes.push_back(box);
	//box->AddPointLight(pointLight_1);
	//box->AddPointLight(pointLight_2);
	//box->AddPointLight(pointLight_3);

	//CMyBox::SetDirectLight(directLight);
	//CMyBox::SetSpotLight(spotLight);

	////box = new CMyBox();
	////box->SetPosition(glm::vec3(1.0f, 0.0f, -1.0f));
	////box->SetRotation(glm::vec3(0.3f, 1.0f, 0.2f), 15);
	////box->SetCamera(camera);
	////boxes.push_back(box);
	////box->AddPointLight(pointLight_1);
	////box->AddPointLight(pointLight_2);
	////box->AddPointLight(pointLight_3);


	////box = new CMyBox();
	////box->SetPosition(glm::vec3(2.0f, 1.0f, -1.5f));
	////box->SetRotation(glm::vec3(3.3f, 1.0f, 1.2f), 90);
	////box->SetCamera(camera);
	////boxes.push_back(box);
	////box->AddPointLight(pointLight_1);
	////box->AddPointLight(pointLight_2);
	////box->AddPointLight(pointLight_3);


	////box = new CMyBox();
	////box->SetPosition(glm::vec3(-1.0f, -1.0f, -2.0f));
	////box->SetCamera(camera);
	////boxes.push_back(box);
	////box->AddPointLight(pointLight_1);
	////box->AddPointLight(pointLight_2);
	////box->AddPointLight(pointLight_3);

	////��ʼ��ģ��
	//model = new CMyModel("../bin/res/model/nanosuit.obj");
	//model->SetCamera(camera);
	//model->SetPosition(glm::vec3(2.0f, 1.0f, -0.5f));
	//model->SetScale(glm::vec3(0.5f));
	//CMyModel::SetDirectLight(directLight);
	//PT_LIGHT pt1;
	//pt1.position = glm::vec3(1.0f, 2.2f, -0.3f);
	//pt1.ambient = glm::vec3(0.0f, 0.0f, 0.0f);
	//pt1.diffuse = glm::vec3(0.0f);
	//pt1.specular = glm::vec3(0.0f, 1.0f, 0.0f);
	//pt1.constant = 1.0f;
	//pt1.linear = 0.4f;
	//pt1.quadratic = 0.9f;

	//PT_LIGHT pt2;
	//pt2.position = glm::vec3(1.0f, 5.7f, 0.3f);
	//pt2.ambient = glm::vec3(0.1f);
	//pt2.diffuse = glm::vec3(0.3f);
	//pt2.specular = glm::vec3(0.0f, 1.0f, 1.0f);
	//pt2.constant = 1.0f;
	//pt2.linear = 0.35f;
	//pt2.quadratic = 0.44f;

	//model->AddPointLight(pt1);
	//model->AddPointLight(pt2);

	//S_LIGHT spotLight;
	//spotLight.ambient = glm::vec3(0.0f);
	//spotLight.diffuse = glm::vec3(0.8f);
	//spotLight.specular = glm::vec3(1.0f);
	//spotLight.constant = 1.0f;
	//spotLight.linear = 0.35f;
	//spotLight.quadratic = 0.44f;
	//spotLight.cutoff = cos(glm::radians(12.0f));
	//spotLight.outCutoff = cos(glm::radians(20.0f));
	//model->SetSpotLight(spotLight);

	//����������
	CreateCube();
}

void InitPoints()
{
	GLfloat points[] = {
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		-1.2f, 0.0f, 0.0f,
	};

	glGenVertexArrays(1, &pointsVAO);
	glGenBuffers(1, &pointsVBO);
	glBindVertexArray(pointsVAO);
	glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glBindVertexArray(0);

	GLfloat points2[] = {
		0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		-1.2f, 0.0f, 1.0f,
	};

	glGenVertexArrays(1, &pointsVAO2);
	glGenBuffers(1, &pointsVBO2);
	glBindVertexArray(pointsVAO2);
	glBindBuffer(GL_ARRAY_BUFFER, pointsVBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points2), points2, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glBindVertexArray(0);
}

void DrawPoints()
{
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_PROGRAM_POINT_SIZE);
	static glm::mat4 matModel;
	glUseProgram(pointShaderProgram);
	matModel = glm::mat4();
	glUniformMatrix4fv(glGetUniformLocation(pointShaderProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(matModel));
	glUniformMatrix4fv(glGetUniformLocation(pointShaderProgram, "matView"), 1, GL_FALSE, glm::value_ptr(*camera->GetView()));
	glUniformMatrix4fv(glGetUniformLocation(pointShaderProgram, "matProjection"), 1, GL_FALSE, glm::value_ptr(*camera->GetProjection()));
	glBindVertexArray(pointsVAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);


	glUseProgram(pointShaderProgram2);
	matModel = glm::mat4();
	glUniformMatrix4fv(glGetUniformLocation(pointShaderProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(matModel));
	glUniformMatrix4fv(glGetUniformLocation(pointShaderProgram, "matView"), 1, GL_FALSE, glm::value_ptr(*camera->GetView()));
	glUniformMatrix4fv(glGetUniformLocation(pointShaderProgram, "matProjection"), 1, GL_FALSE, glm::value_ptr(*camera->GetProjection()));
	glBindVertexArray(pointsVAO2);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
}

void CreateCube()
{
	GLfloat cubeVertices[] = {
		-0.5f, -0.5f, -0.5f,
		0.5f, 0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, 0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, 0.5f, -0.5f,

		-0.5f, -0.5f, 0.5f,
		0.5f, -0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,
		-0.5f, 0.5f, 0.5f,
		-0.5f, -0.5f, 0.5f,

		-0.5f, 0.5f, 0.5f,
		-0.5f, 0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, 0.5f,
		-0.5f, 0.5f, 0.5f,

		0.5f, 0.5f, 0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, 0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, 0.5f, 0.5f,
		0.5f, -0.5f, 0.5f,

		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, 0.5f,
		0.5f, -0.5f, 0.5f,
		-0.5f, -0.5f, 0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, 0.5f, -0.5f,
		0.5f, 0.5f, 0.5f,
		0.5f, 0.5f, -0.5f,
		0.5f, 0.5f, 0.5f,
		-0.5f, 0.5f, -0.5f,
		-0.5f, 0.5f, 0.5f
	};

	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 3, 0);
	glBindVertexArray(0);
}

void DrawCubes()
{
	glUseProgram(colorShaderProgram);
	static glm::mat4 matModel;
	glBindVertexArray(cubeVAO);

	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 16, 64, glm::value_ptr(*camera->GetView()));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	matModel = glm::mat4();
	matModel = glm::translate(matModel, glm::vec3(0.75f, 0.75f, 0.0f));
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 16, glm::value_ptr(glm::vec3(1.0f, 0.0f, 0.0f)));
	glBufferSubData(GL_UNIFORM_BUFFER, 144, 64, glm::value_ptr(matModel));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	matModel = glm::mat4();
	matModel = glm::translate(matModel, glm::vec3(0.75f, -0.75f, 0.0f));
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 16, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.0f)));
	glBufferSubData(GL_UNIFORM_BUFFER, 144, 64, glm::value_ptr(matModel));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	//matModel = glm::mat4();
	//matModel = glm::translate(matModel, glm::vec3(-0.75f, 0.75f, 0.0f));
	//glBufferSubData(GL_UNIFORM_BUFFER, 0, 16, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.0f)));
	//glBufferSubData(GL_UNIFORM_BUFFER, 144, 64, glm::value_ptr(matModel));
	//glBindBuffer(GL_UNIFORM_BUFFER, 0);
	//glUseProgram(colorShaderProgram);
	//glDrawArrays(GL_TRIANGLES, 0, 36);
	//glUseProgram(0);

	glBindVertexArray(0);
}

void BindUniformBuffer()
{
	//auto colorIdx = glGetUniformBlockIndex(colorShaderProgram, "Matries");
	//glUniformBlockBinding(colorShaderProgram, colorIdx, 0);
	glGenBuffers(1, &UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferData(GL_UNIFORM_BUFFER, 16 + 64 + 64 + 64, nullptr, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 80, 64, glm::value_ptr(*camera->GetProjection()));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}