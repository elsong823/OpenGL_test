#pragma once
class CMyRenderTexture
{
public:
	CMyRenderTexture(GLint _width, GLint _height, GLboolean _depth);
	~CMyRenderTexture();
private:
	CMyRenderTexture();

public:
	void Bind();
	GLuint GetTexture();
	void Draw(GLuint _shaderProgram, int _type, int _range);

private:
	void Setup();

private:
	static GLuint VAO;
	static GLuint VBO;
	GLuint m_width;
	GLuint m_height;
	GLboolean m_depth;
	GLuint m_textrue;
	GLuint m_FBO;
	GLuint m_RBO;
};

