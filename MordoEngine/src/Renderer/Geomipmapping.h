#pragma once
#include "Renderer.h"
#include "../Terrain/Terrain.h"

class Geomipmapping : public Renderer
{
public:
	Geomipmapping(const terrain::Terrain& terrain, int patchSize);
	void SetTextureScale(float scale);
	void SetHeightThresholds(float threshold1, float threshold2);
	void Render(const Shader& shader) override;
	~Geomipmapping();
private:
	int m_Width;
	int m_Depth;
	int m_PatchSize;
	GLuint m_Vao;
	GLuint m_Vbo;
	GLuint m_Ebo;
	std::string m_Texture1ID;
	std::string m_Texture2ID;
	std::string m_Texture3ID;
	float m_TextureScale = 50.0f;
	float m_HeightThreshold1 = 0.3f;
	float m_HeightThreshold2 = 0.7f;
	void CreateGeomipGrid(const terrain::Terrain& terrain);
	void CreateGLState();
	void PopulateBuffers(const terrain::Terrain& terrain);
	void InitVertices(const terrain::Terrain& terrain, std::vector<terrain::Vertex>& vertices);
	void InitIndices(std::vector<unsigned int>& indices);
	void CalculateSmoothNormals(std::vector<terrain::Vertex>& vertices, std::vector<unsigned int>& indices);
	unsigned int AddTriangle(unsigned index, std::vector<unsigned>& indices, unsigned v1, unsigned v2, unsigned v3);
};
