
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
char framePerSec[32] = {};

bool keyStatus[128] = {};
GLint drawMode = GL_FILL;
CMyCamera * camera = nullptr;
map<GLchar, stCharacter> chars;
GLuint fontShaderProgram = 0;
GLuint fontVAO, fontVBO;
glm::mat4 matFontProjection;


void Init();
void InitObject();
void Exit();
void Draw();
void Update(GLfloat _dt);
void CheckKeyStatus();
void KeyCallback(GLFWwindow * _win, int _key, int _scanCode, int _action, int _mode);
void CursorCallback(GLFWwindow * _win, double _offsetX, double _offsetY);
void InitFont();
void RenderText(GLuint _shader, string _text, GLfloat _x, GLfloat _y, GLfloat _scale, glm::vec3 _color);

//**********************************

//渲染四边形
GLuint quadShaderProgram = 0;
void RenderQuad();

//渲染场景
GLuint normalShaderProgram = 0;
void RenderFloor();
void RenderModel();
void RenderNormalScene();

//创建gBuffer
GLuint GFBO = 0;
GLuint gPositionDepth = 0;	//位置、深度
GLuint gNormal = 0; //法线
GLuint gAlbedo = 0;	//漫反射率
GLuint GRBO = 0;
void SetupGBuffer();

//test
bool showTest = false;
GLint testType = 4;

//SSAO半球采样点
const int SAMPLE_NUM = 64;
vector<glm::vec3> ssaoKernel;
GLfloat lerp(GLfloat a, GLfloat b, GLfloat f);
void SetupSSAOKernel();
//旋转点
vector<glm::vec3> ssaoNoise;
//记录旋转点的图片
GLuint texNoise;

//ssaoFBO
GLuint ssaoFBO = 0;
GLuint texSSAO = 0;
GLuint ssaoShaderProgram = 0;
void SetupSSAOFBO();

//ssaoBlurFBO
bool blurSSAO = true;
GLuint ssaoBlurFBO = 0;
GLuint texSSAOBlur = 0;
GLuint ssaoBlurShader = 0;
void SetupSSAOBlurFBO();

//灯相关
glm::vec3 lightPos(2.0, 4.0, -2.0);
glm::vec3 lightColor(0.2, 0.2, 0.7);
const GLfloat constant = 1.0; 
const GLfloat linear = 0.09;
const GLfloat quadratic = 0.032;

//最终
GLuint finShaderProgram = 0;
GLboolean useSSAO = 1;
void FinDraw();
//**********************************

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

		//打印帧数
		sprintf_s(framePerSec, 32, "Frame:%.1f", 1.0f / elapsedTime);
		//printf_s("%.5f\n",1.0f / elapsedTime);
		lastTime = curTime;
		glfwPollEvents();
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

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
	else if (_key == GLFW_KEY_A
		|| _key == GLFW_KEY_S
		|| _key == GLFW_KEY_D
		|| _key == GLFW_KEY_W
		|| _key == GLFW_KEY_Q
		|| _key == GLFW_KEY_E
			)
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
	else if (_key == GLFW_KEY_Z && _action == GLFW_RELEASE)
	{
		showTest = !showTest;
		printf_s("show test? %d\n", showTest);
	}
	else if (_key >= GLFW_KEY_4 
		&& _key <= GLFW_KEY_7
		&& _action == GLFW_RELEASE
		)
	{
		testType = _key - GLFW_KEY_0;
		printf_s("test type = %d\n", testType);
	}
	else if (_key == GLFW_KEY_X && _action == GLFW_RELEASE)
	{
		blurSSAO = !blurSSAO;
		printf_s("blur ssao? %d\n", blurSSAO);
	}
	else if (_key == GLFW_KEY_C && _action == GLFW_RELEASE)
	{
		if (useSSAO)
		{
			useSSAO = 0;
		}
		else
		{
			useSSAO = 1;
		}
		printf_s("use ssao? %d\n", useSSAO);
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
	glDisable(GL_BLEND);
	glPolygonMode(GL_FRONT_AND_BACK, drawMode);
	glBindFramebuffer(GL_FRAMEBUFFER, GFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//绘制到fbo中
	RenderNormalScene();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//绘制到ssao fbo中
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(ssaoShaderProgram);
	auto x = glGetUniformLocation(ssaoShaderProgram, "gPositionDepth");
	glUniform1i(glGetUniformLocation(ssaoShaderProgram, "gPositionDepth"), 0);
	glUniform1i(glGetUniformLocation(ssaoShaderProgram, "gNormal"), 1);
	glUniform1i(glGetUniformLocation(ssaoShaderProgram, "texNoise"), 2);
	glUniformMatrix4fv(glGetUniformLocation(ssaoShaderProgram, "matProjection"), 1, GL_FALSE, glm::value_ptr(*camera->GetProjection()));
	char name[32] = {};
	for (int i = 0; i < ssaoKernel.size(); ++i)
	{
		sprintf_s(name, 32, "samples[%d]", i);
		glUniform3fv(glGetUniformLocation(ssaoShaderProgram, name), 1, glm::value_ptr(ssaoKernel[i]));
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPositionDepth);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texNoise);
	//调用一次draw绘制到ssaoFbo中
	RenderQuad();
	//到此，绘制的ssao tex 是包含噪声的，还需要进行一次blur
	glBindFramebuffer(GL_FRAMEBUFFER, 0	);

	//blur
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(ssaoBlurShader);
	glUniform1i(glGetUniformLocation(ssaoBlurShader, "texSSAO"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texSSAO);
	RenderQuad();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (false)
	{
		//如果在测试模式下
		GLuint showTex = gPositionDepth;
		GLint showDepth = 0;
		GLint showPos = 0;
		switch (testType)
		{
		case 4:
			//位置
			showTex = gPositionDepth;
			showPos = 1;
			break;
		case 5:
			//法线
			showTex = gNormal;
			break;
		case 6:
			//反射率
			showTex = gAlbedo;
			break;
		case 7:
			//深度
			showTex = gPositionDepth;
			showDepth = 1;
			break;
		default:
			break;
		}

		glUseProgram(quadShaderProgram);
		glUniform1i(glGetUniformLocation(quadShaderProgram, "sprite"), 0);
		glUniform1i(glGetUniformLocation(quadShaderProgram, "showDepth"), showDepth);
		glUniform1i(glGetUniformLocation(quadShaderProgram, "showPos"), showPos);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, showTex);
		RenderQuad();
	}

	//test
	//glUseProgram(quadShaderProgram);
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, texSSAO);
	//glUniform1i(glGetUniformLocation(quadShaderProgram, "sprite"), 0);
	//RenderQuad();

	//绘制最终效果
	glClear(GL_COLOR_BUFFER_BIT);
	FinDraw();

	//绘制帧数
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	RenderText(fontShaderProgram, framePerSec, 10.0f, 5.0f, 0.3f, glm::vec3(1.0f, 1.0f, 0.0f));
	glEnable(GL_DEPTH_TEST);
}

void InitObject()
{
	//初始化字体
	InitFont();
	//初始化相机
	camera = new CMyCamera(45.0f, WIN_WIDTH, WIN_HEIGHT, glm::vec3(0.0f, 1.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	//创建shader
	normalShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/normalRender.vert", "../bin/shaders/normalRender.frag");
	quadShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/quad.vert", "../bin/shaders/quad.frag");
	ssaoShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/ssao.vert", "../bin/shaders/ssao.frag");
	ssaoBlurShader = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/ssaoBlur.vert", "../bin/shaders/ssaoBlur.frag");
	finShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/ssaoFin.vert", "../bin/shaders/ssaoFin.frag");
	SetupGBuffer();
	SetupSSAOKernel();
	SetupSSAOFBO();
	//用于虚化ssao的结果(应为有噪声)
	SetupSSAOBlurFBO();
}

//初始化字体
void InitFont()
{
	//初始化库
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		printf_s("init freetype library failed!\n");
	//创建面
	FT_Face face;
	if (FT_New_Face(ft, "../bin/res/fonts/arial.ttf", 0, &face))
		printf_s("create ft font failed\n");
	//设定大小
	FT_Set_Pixel_Sizes(face, 0, 48);
	//将字符读入face
	if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
		printf_s("FT load char failed\n");

	//禁用字节对齐限制
	//OpenGL要求所有纹理都必须是4字节对齐的
	//即是必须是4的整数倍
	//但是由于字体纹理只保存了1个字节即8位灰度图
	//所以必须要设置纹理解压对齐参数为1，否则会出对齐错误(段错误)
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	for (int ch = 0; ch < 128; ++ch)
	{
		if (FT_Load_Char(face, ch, FT_LOAD_RENDER))
		{
			printf_s("Load char %c failed\n", ch);
			continue;
		}
		//生成纹理
		GLuint texture = 0;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		//读取信息
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
			face->glyph->bitmap.width,	//字符宽度
			face->glyph->bitmap.rows,	//字符高度
			0,
			GL_RED,GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer	//字符缓冲区内的信息
			);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//储存字符
		stCharacter character;
		character.textureID = texture;
		character.size.x = face->glyph->bitmap.width;
		character.size.y = face->glyph->bitmap.rows;
		character.bearing.x = face->glyph->bitmap_left;
		character.bearing.y = face->glyph->bitmap_top;
		character.advance = face->glyph->advance.x;
		//存入map
		chars.emplace(make_pair(ch, character));
	}
	//释放face 和 freeType
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
	//创建文字shader
	fontShaderProgram = CMyShader::GetInstance()->CreateShaderProgram("../bin/shaders/font.vert", "../bin/shaders/font.frag");
	//字体投影矩阵
	matFontProjection = glm::ortho(0.0f, (GLfloat)WIN_WIDTH, 0.0f, (GLfloat)WIN_HEIGHT);
	//初始化vao, vbo
	glGenVertexArrays(1, &fontVAO);
	glGenBuffers(1, &fontVBO);
	glBindVertexArray(fontVAO);
	glBindBuffer(GL_ARRAY_BUFFER, fontVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* 4 * 6, nullptr, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 4, 0);
	glBindVertexArray(0);
}

//绘制字体
void RenderText(GLuint _shader, string _text, GLfloat _x, GLfloat _y, GLfloat _scale, glm::vec3 _color)
{
	glUseProgram(_shader);
	glUniformMatrix4fv(glGetUniformLocation(_shader, "matProjection"), 1, GL_FALSE, glm::value_ptr(matFontProjection));
	glUniform3fv(glGetUniformLocation(_shader, "textColor"), 1, glm::value_ptr(_color));
	glUniform1i(glGetUniformLocation(_shader, "texText"), 0);
	
	glBindVertexArray(fontVAO);
	glActiveTexture(GL_TEXTURE0);
	// 遍历文本中所有的字符
	std::string::const_iterator c;
	for (c = _text.begin(); c != _text.end(); c++)
	{
		stCharacter ch = chars[*c];

		//计算左下角位置
		GLfloat xpos = _x + ch.bearing.x * _scale;
		//这里要特别注意：一些字符如p,q,g需要被渲染到基准线以下。
		GLfloat ypos = _y - (ch.size.y - ch.bearing.y) * _scale;
		//计算宽高
		GLfloat w = ch.size.x * _scale;
		GLfloat h = ch.size.y * _scale;
		// 对每个字符更新VBO
		GLfloat vertices[6][4] = {
			{ xpos, ypos + h, 0.0, 0.0 },
			{ xpos, ypos, 0.0, 1.0 },
			{ xpos + w, ypos, 1.0, 1.0 },

			{ xpos, ypos + h, 0.0, 0.0 },
			{ xpos + w, ypos, 1.0, 1.0 },
			{ xpos + w, ypos + h, 1.0, 0.0 }
		};
		// 在四边形上绘制字形纹理
		glBindTexture(GL_TEXTURE_2D, ch.textureID);
		// 更新VBO内存的内容
		glBindBuffer(GL_ARRAY_BUFFER, fontVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// 绘制四边形
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// 更新位置到下一个字形的原点，注意单位是1/64像素，因此要除以64， 但是进行位操作更快！
		_x += (ch.advance >> 6) * _scale; // 位偏移6个单位来获取单位为像素的值 (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderNormalScene()
{
	RenderModel();
	RenderFloor();
}

void RenderFloor()
{
	static CMyCube * floorCube = nullptr;
	if (!floorCube)
	{
		floorCube = new CMyCube();
		floorCube->SetCamera(camera);
		floorCube->SetPosition(glm::vec3(0.0, -1.0f, 0.0f));
		floorCube->SetScale(glm::vec3(20.0f, 1.0f, 20.0f));
		floorCube->Update(0.0f);
	}
	floorCube->Draw(normalShaderProgram);
}

void RenderModel()
{
	static CMyModel * model = nullptr;
	if (!model)
	{
		model = new CMyModel("../bin/res/model/nanosuit.obj");
		model->SetCamera(camera);
		model->SetPosition(glm::vec3(0.0f, 0.0f, 5.0));
		model->SetScale(glm::vec3(0.5f));
		model->SetRotation(glm::vec3(1.0, 0.0, 0.0), -90.0f);
		model->Update(0.0f);
	}
	model->Draw(normalShaderProgram);
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
}

void SetupGBuffer()
{
	glGenTextures(1, &gPositionDepth);
	glBindTexture(GL_TEXTURE_2D, gPositionDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glGenTextures(1, &gAlbedo);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenRenderbuffers(1, &GRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, GRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIN_WIDTH, WIN_HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glGenFramebuffers(1, &GFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, GFBO);
	//绑depth
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, GRBO);
	//绑color
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPositionDepth, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);
	//告诉OGL一下有三个color
	GLenum attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf_s("Create fbo failed\n");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SetupSSAOKernel()
{
	//创建随机浮点数分配器
	uniform_real_distribution<float> randomFloats(0.0f, 1.0f);
	//创建随机生成器
	default_random_engine engine;
	for (int i = 0; i < SAMPLE_NUM; ++i)
	{
		//因为是在切线空间，法线必朝上
		//生成随机点x:-1.0f~1.0f y:-1.0f~1.0f z:0.0f~1.0f
		glm::vec3 sample(
			randomFloats(engine)*2.0f - 1.0f, 
			randomFloats(engine)*2.0f - 1.0f, 
			randomFloats(engine));	//半球，z > 0.0f
		//到此，所有的点都是平均分布在半圆空间内的
		sample = glm::normalize(sample);
		//我们希望点越来越接近原点，以采样到真正靠近片段的遮蔽点
		GLfloat scale = GLfloat(i) / 64.0f;
		scale = lerp(0.0f, 1.0f, scale);
		sample *= scale;
		//printf_s("x=%.1f,y=%.1f,z=%.1f\n",sample.x, sample.y, sample.z);
		ssaoKernel.emplace_back(sample);
	}

	//因为这个随机数一旦生成就不会再变
	//为了增加随机性，加入一个转动核心：在切线空间，沿着z轴转动
	//但是为每一个片段增加一个转动核心，代价太高，因此做一个4x4的贴图，每个像素存一个转动值，然后使用repeat方式
	for (int i = 0; i < 16; ++i)
	{
		ssaoNoise.emplace_back(
			randomFloats(engine) * 2.0f - 1.0f,
			randomFloats(engine) * 2.0f - 1.0f,
			0.0f
			);
	}
	//生成贴图
	glGenTextures(1, &texNoise);
	glBindTexture(GL_TEXTURE_2D, texNoise);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLfloat lerp(GLfloat a, GLfloat b, GLfloat f)
{
	return a + (b - a)*f;
}

void SetupSSAOFBO()
{
	glGenTextures(1, &texSSAO);
	glBindTexture(GL_TEXTURE_2D, texSSAO);
	//屏蔽结果就是一个灰度值，因此只需要纹理的红色分量即可
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, WIN_WIDTH, WIN_HEIGHT, 0, GL_RED, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &ssaoFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texSSAO, 0);
	auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf_s("Create ssao fbo failed\n");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SetupSSAOBlurFBO()
{
	glGenTextures(1, &texSSAOBlur);
	glBindTexture(GL_TEXTURE_2D, texSSAOBlur);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, WIN_WIDTH, WIN_HEIGHT, 0, GL_RED, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);


	glGenFramebuffers(1, &ssaoBlurFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texSSAOBlur, 0);
	auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf_s("Create ssao fbo failed\n");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FinDraw()
{
	glUseProgram(finShaderProgram);
	//绑定纹理
	glUniform1i(glGetUniformLocation(finShaderProgram, "gPositionDepth"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPositionDepth);
	glUniform1i(glGetUniformLocation(finShaderProgram, "gNormal"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glUniform1i(glGetUniformLocation(finShaderProgram, "gAlbedo"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glUniform1i(glGetUniformLocation(finShaderProgram, "ssao"), 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, texSSAOBlur);
	//设置灯
	auto pos = glm::vec3( *camera->GetView() * glm::vec4(lightPos, 1.0f));
	//这里传入的一定是变换到观察者空间的灯光位置
	glUniform3fv(glGetUniformLocation(finShaderProgram, "light.position"), 1, glm::value_ptr(pos));
	glUniform3fv(glGetUniformLocation(finShaderProgram, "light.color"), 1, glm::value_ptr(lightColor));
	glUniform1f(glGetUniformLocation(finShaderProgram, "light.constant"), constant);
	glUniform1f(glGetUniformLocation(finShaderProgram, "light.linear"), linear);
	glUniform1f(glGetUniformLocation(finShaderProgram, "light.quadratic"), quadratic);
	glUniform1i(glGetUniformLocation(finShaderProgram, "useSsao"), useSSAO);

	RenderQuad();
}
