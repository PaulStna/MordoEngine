#pragma once
#include <glad/glad.h>

class SkyBoxRenderer
{
private:
	GLuint m_Vao;
	GLuint m_Vbo;
	void CreateGLState();
	void PopulateBuffers();

public:
	SkyBoxRenderer();
	void Render();
	~SkyBoxRenderer();
};