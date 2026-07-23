#pragma once
#include <glad/glad.h>

class Framebuffer
{
private:
	GLuint m_Fbo;
	GLuint m_TextureColorBuffer;
	GLuint m_Rbo;
	int m_Width;
	int m_Height;
	void CreateGLState();

public:
	Framebuffer(int width = 800, int height = 800);
	void BindBuffer();
	void UnbindBuffer();
	GLuint GetTextureID() const { return m_TextureColorBuffer; }
	~Framebuffer();
};
