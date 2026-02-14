#pragma once
#include "Renderer.h"
#include "../Camera/Camera.h"
#include "../Terrain/Terrain.h"
#include "Renderer.h"
#include <memory>
#include <string>

class AreaSelectorRenderer : public Renderer
{
private:
	GLuint m_Vao;
	GLuint m_PosVbo;
	GLuint m_HeightVbo;
	int m_Segments;
	glm::vec3 m_WorldPosition;
	void CreateGLState();
	void PopulateBuffers(float radius);
	void InitVertices(std::vector<glm::vec2>& vertices, float radius);
public:
	AreaSelectorRenderer(float radius, int segments);
	void Render(const Shader& shader, const glm::vec3& cameraPos) override;
	void SetHeights(const std::vector<float>& heights, const glm::vec3& position) override;
	~AreaSelectorRenderer();
};
