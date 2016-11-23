

#include "Utility.h"
#include "MyShader.h"
#include "MyCamera.h"

GLFWwindow * window = nullptr;
const GLint WIN_WIDTH = 800;
const GLint WIN_HEIGHT = 600;
const GLint FRAME = 60;

bool keyStatus[128] = {};
GLint drawMode = GL_FILL;
CMyCamera * camera = nullptr;

GLuint cubeShaderProgram = 0;
GLuint outlineCubeShaderProgram = 0;
GLuint planeShaderProgram = 0;
GLuint vegetationShaderProgram = 0;
GLuint screenShaderProgram = 0;
GLuint mirrorShaderProgram = 0;
D_LIGHT dLight;

GLuint FBO = 0;
GLuint RBO = 0;
GLuint texColorBuffer = 0;
GLuint mirrorVAO = 0;
GLuint mirrorVBO = 0;
GLuint quadVAO = 0;
GLuint quadVBO = 0;
GLuint texMirror = 0;

vector<glm::vec3> vegetationPos;
VEGETATION vegetationItem;
map<GLfloat, glm::vec3> sortedVegetationPos;

vector<MYCUBE> cubes;
MYPLANE plane;

GLuint testFBO = 0;
GLuint testRBO1 = 0;
GLuint testRBO2 = 0;

void Init();
void InitObject();
void Exit();
void Draw();
void DrawNormalCube();
void DrawOutLineCube();
void DrawPlane();
void DrawVegetation();
void DrawFrameBuffer();
void DrawMirror();
void Update(GLfloat _dt);
void CheckKeyStatus();
void KeyCallback(GLFWwindow * _win, int _key, int _scanCode, int _action, int _mode);
void CursorCallback(GLFWwindow * _win, double _offsetX, double _offsetY);
void Test();

//初始化帧缓冲
void CreateMirror();
GLuint CreateAttachmentTexture(bool _depth, bool _stencil);
//创建方块
void CreateCube(glm::vec3 _pos, glm::vec3 _scale, string _texName);
//创建平台
void CreatePlane(glm::vec3 _pos);
//排序草丛
bool FuncSortVeg(glm::vec3 & pos1, glm::vec3 & pos2);
//创建草丛
void CreateVegetation();

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

		Update(elapsedTime);

		//写入镜子
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

		//camera->TurnFace();
		Draw();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glBindFramebuffer(GL_FRAMEBUFFER, testFBO);

		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

		//camera->RotateCamera(glm::radians(-180.0f), 0.0f);
		//camera->TurnFace();
		//普通渲染
		Draw();

		//绘制镜子
		DrawMirror();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glStencilMask(0xFF);

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

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);

	glEnable(GL_CULL_FACE);

	glViewport(0, 0, _width, _height);
	//初始化对象
	InitObject();

	Test();
}

void Exit()
{
	glfwTerminate();
	glDeleteFramebuffers(1, &FBO);
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
	//sortedVegetationPos.clear();
	//auto cameraPos = camera->GetPosition();
	//for (int i = 0; i < vegetationPos.size(); ++i)
	//{
	//	sortedVegetationPos.emplace(make_pair(glm::length(*cameraPos - vegetationPos[i]), vegetationPos[i]));
	//}
	sort(vegetationPos.begin(), vegetationPos.end(), FuncSortVeg);
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
	//绘制立方体
	DrawNormalCube();
	//绘制描边
	DrawOutLineCube();
	//绘制平面
	DrawPlane();
	//绘制草
	DrawVegetation();
}

void InitObject()
{
	//初始化平行光照
	dLight.direction = glm::vec3(-1.0f, 0.0f, 0.0f);
	dLight.ambient = glm::vec3(0.3f);
	dLight.diffuse = glm::vec3(0.7f);
	dLight.specular = glm::vec3(1.0f);

	//初始化shader
	cubeShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/cube.vert","../bin/shaders/cube.frag");
	planeShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/plane.vert", "../bin/shaders/plane.frag");
	outlineCubeShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/cube.vert", "../bin/shaders/outlineCube.frag");
	vegetationShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/vegetation.vert","../bin/shaders/vegetation.frag");
	screenShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/frameBuffer.vert", "../bin/shaders/frameBuffer.frag");
	mirrorShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/mirror.vert", "../bin/shaders/mirror.frag");

	//初始化相机
	camera = new CMyCamera(45.0f, WIN_WIDTH, WIN_HEIGHT, glm::vec3(0.0f, 1.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	//初始化一个方形
	CreateCube(glm::vec3(-1.0f, 0.001f, -1.0f), glm::vec3(1.0f), "container.jpg");
	CreateCube(glm::vec3(2.0f, 0.01f, 0.0f), glm::vec3(1.0f), "container.jpg");

	//初始化一个平面
	CreatePlane(glm::vec3(0.0f, -0.5f, 0.0f));

	//创建草地
	CreateVegetation();


	//初始化帧缓冲
	CreateMirror();
}

void CreateCube(glm::vec3 _pos, glm::vec3 _scale, string _texName)
{
	GLfloat cubeVertices[] = {
		// Positions           // Normals           // Texture Coords
		0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

		0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

		0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
	};

	MYCUBE cube;
	cube.scale = _scale;
	cube.position = _pos;
	glGenVertexArrays(1, &cube.VAO);
	glGenBuffers(1, &cube.VBO);
	glBindVertexArray(cube.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, cube.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 8, (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 8, (GLvoid*)(sizeof(GLfloat)* 3));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 8, (GLvoid*)(sizeof(GLfloat)* 6));

	glBindVertexArray(0);

	//Load Tex
	cube.material.texDiffuse = LoadTexture("../bin/res/container2.png", false, GL_REPEAT);
	cube.material.texSpecular = LoadTexture("../bin/res/container2_specular.png", false, GL_REPEAT);
	cube.material.shininess = 64;
	cubes.push_back(cube);
}

void CreatePlane(glm::vec3 _pos)
{
	GLfloat planeVertices[] = {
		// Positions            // Texture Coords (note we set these higher than 1 that together with GL_REPEAT as texture wrapping mode will cause the floor texture to repeat)
		-5.0f, -0.5f, -5.0f, 0.0f, 2.0f,
		-5.0f, -0.5f, 5.0f, 0.0f, 0.0f,
		 5.0f, -0.5f, 5.0f, 2.0f, 0.0f,
		 -5.0f, -0.5f, -5.0f, 0.0f, 2.0f,
		5.0f, -0.5f, 5.0f, 2.0f, 0.0f,
		5.0f, -0.5f, -5.0f, 2.0f, 2.0f
	};
	glGenVertexArrays(1, &plane.VAO);
	glGenBuffers(1, &plane.VBO);
	glBindVertexArray(plane.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, plane.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 5, (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 5, (GLvoid*)(sizeof(GLfloat)* 3));

	glBindVertexArray(0);

	plane.texture = LoadTexture("../bin/res/tex_1.jpg", false, GL_REPEAT);
}

void DrawNormalCube()
{	
	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);

	//draw cubes
	glm::mat4 matModel;
	for (auto cube : cubes)
	{
		matModel = glm::mat4();
		matModel = glm::translate(matModel, cube.position);
		matModel = glm::scale(matModel, cube.scale);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cube.material.texDiffuse);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, cube.material.texSpecular);

		glUseProgram(cubeShaderProgram);
		glUniformMatrix4fv(glGetUniformLocation(cubeShaderProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(matModel));
		glUniformMatrix4fv(glGetUniformLocation(cubeShaderProgram, "matView"), 1, GL_FALSE, glm::value_ptr(*camera->GetView()));
		glUniformMatrix4fv(glGetUniformLocation(cubeShaderProgram, "matProjection"), 1, GL_FALSE, glm::value_ptr(*camera->GetProjection()));
		glUniform1i(glGetUniformLocation(cubeShaderProgram, "material.texDiffuse"), 0);
		glUniform1i(glGetUniformLocation(cubeShaderProgram, "material.texSpecular"), 1);
		glUniform1f(glGetUniformLocation(cubeShaderProgram, "material.shininess"), cube.material.shininess);
		glUniform3fv(glGetUniformLocation(cubeShaderProgram, "directLight.direction"), 1, glm::value_ptr(dLight.direction));
		glUniform3fv(glGetUniformLocation(cubeShaderProgram, "directLight.ambient"), 1, glm::value_ptr(dLight.ambient));
		glUniform3fv(glGetUniformLocation(cubeShaderProgram, "directLight.diffuse"), 1, glm::value_ptr(dLight.diffuse));
		glUniform3fv(glGetUniformLocation(cubeShaderProgram, "directLight.specular"), 1, glm::value_ptr(dLight.specular));
		glUniform3fv(glGetUniformLocation(cubeShaderProgram, "eyePos"), 1, glm::value_ptr(*camera->GetPosition()));

		glBindVertexArray(cube.VAO);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
	}
}

void DrawOutLineCube()
{
	glStencilMask(0x00);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);

	glUseProgram(outlineCubeShaderProgram);
	//Draw out line cube
	glm::mat4 matModel;
	for (auto cube:cubes)
	{
		matModel = glm::mat4();
		matModel = glm::translate(matModel, cube.position);
		matModel = glm::scale(matModel, cube.scale + glm::vec3(.03f));
		glUniformMatrix4fv(glGetUniformLocation(outlineCubeShaderProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(matModel));
		glUniformMatrix4fv(glGetUniformLocation(outlineCubeShaderProgram, "matView"), 1, GL_FALSE, glm::value_ptr(*camera->GetView()));	
		glUniformMatrix4fv(glGetUniformLocation(outlineCubeShaderProgram, "matProjection"), 1, GL_FALSE, glm::value_ptr(*camera->GetProjection()));
		glBindVertexArray(cube.VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}
	glStencilMask(0xFF);
}

void DrawPlane()
{
	glDisable(GL_CULL_FACE);
	glStencilMask(0x00);
	glStencilFunc(GL_ALWAYS, 1, 0x00);

	glm::mat4 matModel;
	matModel = glm::mat4();
	matModel = glm::translate(matModel, plane.position);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, plane.texture);
	glUseProgram(planeShaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(planeShaderProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(matModel));
	glUniformMatrix4fv(glGetUniformLocation(planeShaderProgram, "matView"), 1, GL_FALSE, glm::value_ptr(*camera->GetView()));
	glUniformMatrix4fv(glGetUniformLocation(planeShaderProgram, "matProjection"), 1, GL_FALSE, glm::value_ptr(*camera->GetProjection()));
	glUniform1i(glGetUniformLocation(planeShaderProgram, "u_fragTexture"), 0);
	glBindVertexArray(plane.VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glEnable(GL_CULL_FACE);
	glStencilMask(0xFF);
}

void DrawVegetation()
{
	glDisable(GL_CULL_FACE);
	glUseProgram(vegetationShaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(vegetationShaderProgram, "matView"), 1, GL_FALSE, glm::value_ptr(*camera->GetView()));
	glUniformMatrix4fv(glGetUniformLocation(vegetationShaderProgram, "matProjection"), 1, GL_FALSE, glm::value_ptr(*camera->GetProjection()));
	glUniform1f(glGetUniformLocation(vegetationShaderProgram, "fragTexture"), vegetationItem.texture);

	glBindVertexArray(vegetationItem.VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, vegetationItem.texture);

	for (auto & item : vegetationPos)
	{
		glm::mat4 matModel = glm::mat4();
		matModel = glm::translate(matModel, item);
		glUniformMatrix4fv(glGetUniformLocation(vegetationShaderProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(matModel));
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glEnable(GL_CULL_FACE);
}

void DrawMirror()
{
	glUseProgram(mirrorShaderProgram);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texMirror);
	glUniform1i(glGetUniformLocation(mirrorShaderProgram, "fragTexture"), 0);
	glBindVertexArray(mirrorVAO);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void CreateVegetation()
{
	vegetationPos.emplace_back(-1.5f,0.0f, -0.48f);
	vegetationPos.emplace_back(1.5f, 0.0f, 0.51f);
	vegetationPos.emplace_back(0.0f, 0.0f, 0.7f);
	vegetationPos.emplace_back(-0.3f, 0.0f, -2.3f);
	vegetationPos.emplace_back(0.5f, 0.0f, -0.6f);

	GLfloat vertices[] = {
		// Positions         // Texture Coords (swapped y coordinates because texture is flipped upside down)
		0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f, 1.0f,
		1.0f, -0.5f, 0.0f, 1.0f, 1.0f,

		0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
		1.0f, -0.5f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.5f, 0.0f, 1.0f, 0.0f
	};
	glGenVertexArrays(1, &vegetationItem.VAO);
	glGenBuffers(1, &vegetationItem.VBO);
	glGenTextures(1, &vegetationItem.texture);

	glBindVertexArray(vegetationItem.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, vegetationItem.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 5, (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 5, (GLvoid*)(sizeof(GLfloat)* 3));
	glBindVertexArray(0);

	vegetationItem.texture = LoadTexture("../bin/res/grass.png", true, GL_CLAMP_TO_BORDER);
}

bool FuncSortVeg(glm::vec3 & pos1, glm::vec3 & pos2)
{
	auto cameraPos = camera->GetPosition();
	return glm::length(*cameraPos - pos1) > glm::length(*cameraPos - pos2);
}

void CreateFrameBuffer()
{
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	//创建纹理图像	
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	//为帧缓冲附上color buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	//创建渲染缓冲
	GLuint renderBuffer = 0;
	glGenRenderbuffers(1, &renderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	//设置渲染缓冲数据结构格式
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIN_WIDTH, WIN_HEIGHT);	
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);

	auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf_s("Bind frame buffer failed\n");
	}

	//解绑，避免渲染到错误的帧缓冲上
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLfloat vertices[] = {
		-1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f,

		-1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	};
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 4, (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 4, (GLvoid*)(sizeof(GLfloat)*2));
	glBindVertexArray(0);
}

void DrawFrameBuffer()
{
	glDisable(GL_CULL_FACE);
	glUseProgram(screenShaderProgram);
	glBindVertexArray(quadVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glUniform1i(glGetUniformLocation(screenShaderProgram, "screenTexture"), 0);
	auto tx = glGetUniformLocation(screenShaderProgram, "screenTexture");
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glEnable(GL_CULL_FACE);
}

void CreateMirror()
{
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	texMirror = CreateAttachmentTexture(false, false);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texMirror, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		printf_s("heheda\n");
	}

	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIN_WIDTH, WIN_HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLfloat vertices[] = {
		-1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, 1.0f, 0.0f, 0.0f,
		 1.0f, 1.0f, 1.0f, 0.0f,

		-1.0f, 1.0f, 0.0f, 1.0f,
		 1.0f, 1.7f, 1.0f, 0.0f,
		 1.0f, 1.0f, 1.0f, 1.0f,
	};
	glGenVertexArrays(1, &mirrorVAO);
	glGenBuffers(1, &mirrorVBO);
	glBindVertexArray(mirrorVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mirrorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 4, (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 4, (GLvoid*)(sizeof(GLfloat)* 2));
	glBindVertexArray(0);
}

GLuint CreateAttachmentTexture(bool _depth, bool _stencil)
{
	GLuint texture = 0;
	GLint type = 0;
	GLint format = 0;
	GLint iFormat = 0;
	if (_depth && _stencil)
	{
		//之所以要写出24和8，因为对于内部格式(internalFormat)而言，不知道深度和模板各占多少位
		iFormat = GL_DEPTH24_STENCIL8;
		format = GL_DEPTH_STENCIL;
		type = GL_UNSIGNED_INT_24_8;
	}
	else if (_depth)
	{
		iFormat = GL_DEPTH_COMPONENT;
		format = GL_DEPTH_COMPONENT;
		type = GL_UNSIGNED_BYTE;
	}
	else if (_stencil)
	{
		iFormat = GL_STENCIL_INDEX;
		format = GL_STENCIL_INDEX;
		type = GL_UNSIGNED_BYTE;
	}
	else
	{
		iFormat = GL_RGB;
		format = GL_RGB;
		type = GL_UNSIGNED_BYTE;
	}
	
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, iFormat, WIN_WIDTH, WIN_HEIGHT, 0, format, type, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

void Test()
{
	glBlitFramebuffer();
	glGenFramebuffers(1, &testFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, testFBO);

	glGenRenderbuffers(1, &testRBO1);
	glGenRenderbuffers(1, &testRBO2);
	glBindRenderbuffer(GL_RENDERBUFFER, testRBO1);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, WIN_WIDTH, WIN_HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, testRBO1);
	
	glBindRenderbuffer(GL_RENDERBUFFER, testRBO2);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIN_WIDTH, WIN_HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, testRBO2);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		printf_s("hehe ");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}