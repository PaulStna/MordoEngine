#pragma once
#include "Renderer.h"
#include "../Terrain/Terrain.h"
#include <string>

class TriangleRenderer : public Renderer
{
private:
	int m_Width;
	int m_Depth;
	GLuint m_Vao;
	GLuint m_Vbo;
	GLuint m_Ebo;
	std::string m_Texture1ID;
	std::string m_Texture2ID;
	std::string m_Texture3ID;
	float m_TextureScale = 50.0f;
	float m_HeightThreshold1 = 0.3f;
	float m_HeightThreshold2 = 0.7f;
	void CreateTriangleList(const terrain::Terrain& terrain);
	void CreateGLState();
	void PopulateBuffers(const terrain::Terrain& terrain);
	void InitVertices(const terrain::Terrain& terrain, std::vector<terrain::Vertex>& vertices);
	void InitIndices(std::vector<unsigned int>& indices);
	void CalculateSmoothNormals(std::vector<terrain::Vertex>& vertices, std::vector<unsigned int>& indices);
public:
	TriangleRenderer(Shader& shader, const terrain::Terrain& terrain);
	void SetTextureScale(float scale);
	void SetHeightThresholds(float threshold1, float threshold2);
	void Render(const glm::vec3& cameraPos) override;
	~TriangleRenderer();
};
