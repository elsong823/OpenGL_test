

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
vector<glm::vec3> vegetationPos;
VEGETATION vegetationItem;
map<GLfloat, glm::vec3> sortedVegetationPos;

vector<MYCUBE> cubes;
MYPLANE plane;

void Init();
void InitObject();
void Exit();
void Draw();
void DrawNormalCube();
void DrawOutLineCube();
void DrawPlane();
void DrawVegetation();
void Update(GLfloat _dt);
void CheckKeyStatus();
void KeyCallback(GLFWwindow * _win, int _key, int _scanCode, int _action, int _mode);
void CursorCallback(GLFWwindow * _win, double _offsetX, double _offsetY);
void CreateCube(glm::vec3 _pos, glm::vec3 _scale);
void CreatePlane(glm::vec3 _pos);
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
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
		Update(elapsedTime);
		Draw();

		glStencilMask(0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
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

	glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
	glBlendEquation(GL_FUNC_ADD);

	glEnable(GL_CULL_FACE);

	glViewport(0, 0, _width, _height);
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

	DrawNormalCube();
	//DrawOutLineCube();
	DrawPlane();
	//绘制草地
	DrawVegetation();
}

void InitObject()
{
	//初始化shader
	cubeShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/cube.vert","../bin/shaders/cube.frag");
	planeShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/plane.vert", "../bin/shaders/plane.frag");
	outlineCubeShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/cube.vert", "../bin/shaders/outlineCube.frag");
	vegetationShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/vegetation.vert","../bin/shaders/vegetation.frag");

	//初始化相机
	camera = new CMyCamera(45.0f, WIN_WIDTH, WIN_HEIGHT, glm::vec3(0.0f, 1.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	//初始化一个方形
	CreateCube(glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(1.0f));
	CreateCube(glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(1.0f));

	//初始化一个平面
	CreatePlane(glm::vec3(0.0f, -0.5f, 0.0f));

	//创建草地
	CreateVegetation();
}

void CreateCube(glm::vec3 _pos, glm::vec3 _scale)
{
	GLfloat cubeVertices[] = {
	// Positions          // Texture Coords
		// Back face
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // Bottom-left
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, // bottom-right    
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right              
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, // top-left
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // bottom-left                
		// Front face
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, // top-right
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, // bottom-right        
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, // top-right
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, // top-left        
		// Left face
		-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // top-right
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-left
		-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-left       
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-left
		-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // top-right
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-right
		// Right face
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // top-left
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right      
		0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-right          
		0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-right
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // top-left
		// Bottom face          
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // top-right
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, // bottom-left
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, // top-left        
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, // bottom-left
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // top-right
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-right
		// Top face
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, // top-left
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // bottom-right                 
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // bottom-right 
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, // bottom-left  
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f  // top-left              
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 5, (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 5, (GLvoid*)(sizeof(GLfloat)*3));

	glBindVertexArray(0);
	cubes.push_back(cube);
}

void CreatePlane(glm::vec3 _pos)
{
	GLfloat planeVertices[] = {
		// Positions            // Texture Coords (note we set these higher than 1 that together with GL_REPEAT as texture wrapping mode will cause the floor texture to repeat)
		5.0f, -0.5f, 5.0f, 2.0f, 0.0f,
		-5.0f, -0.5f, 5.0f, 0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f, 0.0f, 2.0f,

		5.0f, -0.5f, 5.0f, 2.0f, 0.0f,
		-5.0f, -0.5f, -5.0f, 0.0f, 2.0f,
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
}

void DrawNormalCube()
{	
	glFrontFace(GL_CW);
	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);

	//draw cubes
	glm::mat4 matModel;
	for (auto cube : cubes)
	{
		matModel = glm::mat4();
		matModel = glm::translate(matModel, cube.position);
		matModel = glm::scale(matModel, cube.scale);
		glUseProgram(cubeShaderProgram);
		glUniformMatrix4fv(glGetUniformLocation(cubeShaderProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(matModel));
		glUniformMatrix4fv(glGetUniformLocation(cubeShaderProgram, "matView"), 1, GL_FALSE, glm::value_ptr(*camera->GetView()));
		glUniformMatrix4fv(glGetUniformLocation(cubeShaderProgram, "matProjection"), 1, GL_FALSE, glm::value_ptr(*camera->GetProjection()));
		glBindVertexArray(cube.VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}
	glFrontFace(GL_CCW);
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
		matModel = glm::scale(matModel, cube.scale + glm::vec3(.1f));
		glUniformMatrix4fv(glGetUniformLocation(outlineCubeShaderProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(matModel));
		glUniformMatrix4fv(glGetUniformLocation(outlineCubeShaderProgram, "matView"), 1, GL_FALSE, glm::value_ptr(*camera->GetView()));	
		glUniformMatrix4fv(glGetUniformLocation(outlineCubeShaderProgram, "matProjection"), 1, GL_FALSE, glm::value_ptr(*camera->GetProjection()));
		glBindVertexArray(cube.VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}
}

void DrawPlane()
{
	glStencilMask(0x00);
	glStencilFunc(GL_ALWAYS, 1, 0x00);
	glm::mat4 matModel;
	matModel = glm::mat4();
	matModel = glm::translate(matModel, plane.position);
	glUseProgram(planeShaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(cubeShaderProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(matModel));
	glUniformMatrix4fv(glGetUniformLocation(cubeShaderProgram, "matView"), 1, GL_FALSE, glm::value_ptr(*camera->GetView()));
	glUniformMatrix4fv(glGetUniformLocation(cubeShaderProgram, "matProjection"), 1, GL_FALSE, glm::value_ptr(*camera->GetProjection()));
	glBindVertexArray(plane.VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
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

	GLubyte * source = nullptr;
	GLint width, height;
	source = SOIL_load_image("../bin/res/grass.png", &width, &height, nullptr, SOIL_LOAD_RGBA);
	glBindTexture(GL_TEXTURE_2D, vegetationItem.texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, source);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	SOIL_free_image_data(source);
	source = nullptr;
}

bool FuncSortVeg(glm::vec3 & pos1, glm::vec3 & pos2)
{
	auto cameraPos = camera->GetPosition();
	return glm::length(*cameraPos - pos1) > glm::length(*cameraPos - pos2);
}