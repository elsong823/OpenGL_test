
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

	glEnable(GL_BLEND);
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
	RenderText(fontShaderProgram, framePerSec, 10.0f, 5.0f, 0.3f, glm::vec3(1.0f));
	glPolygonMode(GL_FRONT_AND_BACK, drawMode);
}

void InitObject()
{
	//初始化字体
	InitFont();
	//初始化相机
	camera = new CMyCamera(45.0f, WIN_WIDTH, WIN_HEIGHT, glm::vec3(0.0f, 1.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f));


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