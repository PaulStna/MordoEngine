#pragma once
#include "Renderer.h"

class SkyBoxRenderer : public  Renderer
{
private:
	GLuint m_Vao;
	GLuint m_Vbo;
	void CreateGLState();
	void PopulateBuffers();
	void InitVertices(std::vector<float>& vertices);

public:
	SkyBoxRenderer(Shader& shaders);
	void Render(const glm::mat4* view,
		const glm::mat4* projection,
		const glm::mat4* model) override;
	~SkyBoxRenderer();
};