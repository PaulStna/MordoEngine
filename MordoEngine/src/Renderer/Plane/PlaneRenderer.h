#pragma once
#include <glad/glad.h>

class PlaneRenderer
{
private:
	GLuint m_Vao;
	GLuint m_Vbo;
	GLuint m_Ebo;
	void CreateGLState();
	void PopulateBuffers();

public:
	PlaneRenderer();
	void Render();
	~PlaneRenderer();
};