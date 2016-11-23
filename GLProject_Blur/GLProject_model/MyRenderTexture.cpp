#include "Utility.h"
#include "MyRenderTexture.h"

GLuint CMyRenderTexture::VAO = 0;
GLuint CMyRenderTexture::VBO = 0;

CMyRenderTexture::CMyRenderTexture()
{}

CMyRenderTexture::~CMyRenderTexture()
{

}

CMyRenderTexture::CMyRenderTexture(GLint _width, GLint _height, GLboolean _depth)
: m_width(_width)
, m_height(_height)
, m_depth(_depth)
{
	this->Setup();
}

void CMyRenderTexture::Setup()
{
	if (CMyRenderTexture::VAO == 0)
	{
		GLfloat vertices[] = {
			1.0f, -1.0f, 1.0f, 0.0f,
			-1.0f, 1.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f,

			1.0f, -1.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f, 0.0f, 1.0f,
		};
		glGenVertexArrays(1, &CMyRenderTexture::VAO);
		glGenBuffers(1, &CMyRenderTexture::VBO);
		glBindVertexArray(CMyRenderTexture::VAO);
		glBindBuffer(GL_ARRAY_BUFFER, CMyRenderTexture::VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 4, 0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 4, (GLvoid*)(sizeof(GLfloat)* 2));
		glBindVertexArray(0);
	}
	//fbo
	glGenFramebuffers(1, &m_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	//init texture
	glGenTextures(1, &m_textrue);
	glBindTexture(GL_TEXTURE_2D, m_textrue);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textrue, 0);
	//init 
	if (m_depth)
	{
		glGenRenderbuffers(1, &m_RBO);
		glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RBO);
	}
	auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf_s("framebuffer error\n");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CMyRenderTexture::Bind()
{
	if (m_FBO == 0)
		return;
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
}

GLuint CMyRenderTexture::GetTexture()
{
	return m_textrue;
}

void CMyRenderTexture::Draw(GLuint _shaderProgram, int _type, int _range)
{
	glClear(GL_COLOR_BUFFER_BIT);
	if (m_depth)
	{
		glEnable(GL_DEPTH_TEST);
		glClear(GL_DEPTH_BUFFER_BIT);
	}
	glUseProgram(_shaderProgram);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_textrue);
	glUniform1i(glGetUniformLocation(_shaderProgram, "tex"), 0);
	glUniform1i(glGetUniformLocation(_shaderProgram, "type"), _type);
	glUniform1i(glGetUniformLocation(_shaderProgram, "range"), _range);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glDisable(GL_DEPTH_TEST);
}
