
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

//*********************************************
//创建一面墙
GLuint wallVAO = 0;
GLuint wallVBO = 0;
GLuint wallShaderProgram = 0;
GLuint texWall = 0;
GLuint texWallNormal = 0;
GLuint type = 1;	//1顶点法线 2 法线贴图
glm::vec3 tangent1;
glm::vec3 bitangent1;
glm::mat4 wallMatModel;
void CreateWall();
void UPdateWall(GLfloat _dt);
void DrawWall();

//光
S_LIGHT spotLight;
void SetupLight();
void UpdateLight(GLfloat _dt);
//把光绘制出来
void RenderSpotLight();

//创建UBO
GLuint UBO = 0;
void SetupUBO();


//*********************************************
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
	else if (_key == GLFW_KEY_4 && _action == GLFW_RELEASE)
	{
		if (type == 1)
		{	
			type = 2;
			printf_s("Use normal texture\n");
		}
		else
		{
			type = 1;
			printf_s("Use vertex normal\n");
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
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(*camera->GetView()));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	UpdateLight(_dt);
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
	RenderSpotLight();
	DrawWall();
}

void InitObject()
{
	SetupLight();
	SetupUBO();
	//初始化相机
	camera = new CMyCamera(45.0f, WIN_WIDTH, WIN_HEIGHT, glm::vec3(-2.0f, 1.0f, 4.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	//绑定投影矩阵
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(*camera->GetProjection()));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	CreateWall();
}

void CreateWall()
{
	GLfloat vertices[] = 
	{
		-10.0f,  10.0f,  0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 1.0f,
		-10.0f, -10.0f,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		 10.0f,  10.0f,  0.0f, 5.0f, 5.0f, 0.0f, 0.0f, 1.0f,
		 10.0f, -10.0f,  0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	};
	glGenVertexArrays(1, &wallVAO);
	glGenBuffers(1, &wallVBO);
	glBindVertexArray(wallVAO);

	glBindBuffer(GL_ARRAY_BUFFER, wallVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), nullptr, GL_STATIC_DRAW);
	//用获取内存地址的方式复制一次数值
	auto idx = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memcpy(idx, vertices, sizeof(vertices));
	//不能忘记释放
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)* 8, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)* 8, (GLvoid*)(sizeof(GL_FLOAT)* 3));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)* 8, (GLvoid*)(sizeof(GL_FLOAT)* 5));
	glBindVertexArray(0);

	//创建shader
	wallShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/wall.vert", "../bin/shaders/wall.frag");
	auto tag = glGetUniformBlockIndex(wallShaderProgram, "Matrices");
	glUniformBlockBinding(wallShaderProgram, tag, 1);

	//创建tex
	texWall = LoadTexture("../bin/res/wall.png", true);
	texWallNormal = LoadTexture("../bin/res/wall_normal.png", true);

	//计算切线空间
	//点
	glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
	glm::vec3 pos2(-1.0f,-1.0f, 0.0f);
	glm::vec3 pos3( 1.0f, 1.0f, 0.0f);
	glm::vec3 pos4( 1.0f,-1.0f, 0.0f);
	//纹理坐标
	glm::vec2 uv1(0.0f, 1.0f);
	glm::vec2 uv2(0.0f, 0.0f);
	glm::vec2 uv3(1.0f, 1.0f);
	glm::vec2 uv4(1.0f, 0.0f);
	//法线
	glm::vec3 normal(0.0f, 0.0f, 1.0f);

	//计算三角形边 和 deltaUV坐标
	glm::vec3 edge1 = pos1 - pos2;
	glm::vec3 edge2 = pos3 - pos2;
	glm::vec2 deltaUV1 = uv1 - uv2;
	glm::vec2 deltaUV2 = uv3 - uv2;

	//计算 tangent 和 bitangent
	GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
	tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	tangent1 = glm::normalize(tangent1);

	bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
	bitangent1 = glm::normalize(bitangent1);
}

void UPdateWall(GLfloat _dt)
{

}

void DrawWall()
{
	glUseProgram(wallShaderProgram);
	wallMatModel = glm::mat4();
	//让墙面躺下
	wallMatModel = glm::rotate(wallMatModel, glm::radians(-90.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	//wallMatModel = glm::scale(wallMatModel, glm::vec3(0.5f));

	glUniformMatrix4fv(glGetUniformLocation(wallShaderProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(wallMatModel));
	glUniform1i(glGetUniformLocation(wallShaderProgram, "texDiffuse"), 0);
	glUniform1i(glGetUniformLocation(wallShaderProgram, "texNormal"), 1);
	glUniform1i(glGetUniformLocation(wallShaderProgram, "type"), type);
	glUniform3fv(glGetUniformLocation(wallShaderProgram, "spotLight.position"), 1, glm::value_ptr(spotLight.position));
	glUniform3fv(glGetUniformLocation(wallShaderProgram, "spotLight.dir"), 1, glm::value_ptr(spotLight.lightDir));
	glUniform3fv(glGetUniformLocation(wallShaderProgram, "viewPos"), 1, glm::value_ptr(*camera->GetPosition()));
	glUniform1f(glGetUniformLocation(wallShaderProgram, "spotLight.constant"), spotLight.constant);
	glUniform1f(glGetUniformLocation(wallShaderProgram, "spotLight.linear"), spotLight.linear);
	glUniform1f(glGetUniformLocation(wallShaderProgram, "spotLight.quadratic"), spotLight.quadratic);
	glUniform1f(glGetUniformLocation(wallShaderProgram, "spotLight.cutoff"), cos(spotLight.cutoff));
	glUniform1f(glGetUniformLocation(wallShaderProgram, "spotLight.outCutoff"), cos(spotLight.outCutoff));
	glUniform3fv(glGetUniformLocation(wallShaderProgram, "tangent"), 1, glm::value_ptr(tangent1));
	glUniform3fv(glGetUniformLocation(wallShaderProgram, "bitangent"), 1, glm::value_ptr(bitangent1));


	glBindVertexArray(wallVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texWall);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texWallNormal);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);


	//test
	glm::mat4 matModel;
	matModel = glm::translate(matModel, glm::vec3(1,2,3));
	matModel = glm::rotate(matModel, glm::radians(45.0f), glm::vec3(1.0f, 1.0f, 2.0f));
	matModel = glm::scale(matModel, glm::vec3(1.2f));

	//vec
	glm::vec3 xvec(1.0f, 3.0f, 2.5f);

	//trans
	auto x1 = matModel * glm::vec4(xvec, 0.0f);
	auto x2 = glm::transpose(glm::inverse(glm::mat3(matModel))) * xvec;
	int a = 1;
}

void SetupUBO()
{
	glGenBuffers(1, &UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, nullptr, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void SetupLight()
{
	spotLight.position = glm::vec3(0.0f, 0.0f, 2.0f);
	spotLight.cutoff = glm::radians(15.0f);
	spotLight.outCutoff = glm::radians(35.0f);
	spotLight.constant = 1.0f;
	spotLight.linear = 0.09f;
	spotLight.quadratic = 0.032f;
}

void RenderSpotLight()
{
	//light
	static CMyCube * lightPoint = nullptr;
	static GLuint lightShaderProgram = 0;
	static GLuint lightLineShaderProgram = 0;
	if (!lightPoint)
	{
		lightPoint = new CMyCube();
		lightShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/cube.vert", "../bin/shaders/cube.frag");
		auto idx = glGetUniformBlockIndex(lightShaderProgram, "Matrices");
		glUniformBlockBinding(lightShaderProgram, idx, 1);
	}
	lightPoint->SetPosition(spotLight.position);
	lightPoint->SetScale(glm::vec3(0.1f));
	lightPoint->Update(0);
	glDisable(GL_CULL_FACE);
	lightPoint->Draw(lightShaderProgram);
	glEnable(GL_CULL_FACE);

	static GLuint lineVAO = 0;
	static GLuint lineVBO = 0;
	if (!lineVAO)
	{
		glGenVertexArrays(1, &lineVAO);
		glGenBuffers(1, &lineVBO);
		glBindVertexArray(lineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
		glBufferData(GL_ARRAY_BUFFER, 12, nullptr, GL_STATIC_DRAW);
		glBindVertexArray(0);
		lightLineShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/lightLine.vert", "../bin/shaders/lightLine.geo", "../bin/shaders/lightLine.frag");
		auto idx = glGetUniformBlockIndex(lightShaderProgram, "Matrices");
		glUniformBlockBinding(lightShaderProgram, idx, 1);
	}
	glUseProgram(lightLineShaderProgram);
	glUniform3fv(glGetUniformLocation(lightLineShaderProgram, "lightPos"), 1, glm::value_ptr(spotLight.position));
	glUniform3fv(glGetUniformLocation(lightLineShaderProgram, "lightDir"), 1, glm::value_ptr(spotLight.lightDir));
	glBindVertexArray(lineVAO);
	glDrawArrays(GL_POINTS, 0, 1);
	glBindVertexArray(0);
}

void UpdateLight(GLfloat _dt)
{
	static GLfloat timer = 0.0f;
	static GLfloat radius = 2.0f;
	timer += _dt * 0.2f;
	spotLight.position.x = sin(timer) * radius;
	spotLight.position.z = cos(timer) * radius;
	spotLight.position.y = 3.0f;
	spotLight.lightDir = -glm::normalize(spotLight.position);
}