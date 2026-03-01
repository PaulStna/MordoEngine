#pragma once
#include <glad/glad.h>

class Framebuffer 
{
private:
	GLuint m_Fbo;
	GLuint m_TextureColorBuffer;
	GLuint m_Rbo;
	void CreateGLState();

public:
	Framebuffer();
	void BindBuffer();
	void UnbindBuffer();
	GLuint GetTextureID() const { return m_TextureColorBuffer; }
	~Framebuffer();
};
