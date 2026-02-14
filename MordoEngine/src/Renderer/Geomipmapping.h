#pragma once
#include "Renderer.h"
#include <vector>
#include "LodManager.h"
#include "../Terrain/Terrain.h"

class Geomipmapping : public Renderer
{
public:
	Geomipmapping(const terrain::Terrain& terrain, int patchSize);
	void SetTextureScale(float scale);
	void SetHeightThresholds(float threshold1, float threshold2);
	void SetHeights(const std::vector<float>& heights, const glm::vec3& position) override
	{
		return;
	};
	void Render(const Shader& shader, const glm::vec3& cameraPos) override;
	~Geomipmapping();
private:
	int m_Width;
	int m_Depth;
	int m_PatchSize;
	struct SingleLodInfo {
		int start = 0;
		int count = 0;
	};

	struct LodInfo {
		SingleLodInfo info[2][2][2][2];
	};

	std::vector<LodInfo> m_LodInfo;
	int m_NumPatchesX;
	int m_NumPatchesZ;
	int m_MaxLOD;
	LodManager m_LodManager;
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
	int InitIndices(std::vector<unsigned int>& indices);
	void CalculateSmoothNormals(std::vector<terrain::Vertex>& vertices, std::vector<unsigned int>& indices);
	unsigned int AddTriangle(unsigned int index, std::vector<unsigned int>& indices,
		unsigned int vertex1, unsigned int vertex2, unsigned int vertex3);
	int CalcNumIndices();
	int InitIndicesLOD(int index, std::vector<unsigned int>& indices, int lod);
	int InitIndicesLODSingle(int index, std::vector<unsigned int>& indices,
		int lodCore, int lodLeft, int lodRight, int lodTop, int lodBottom);
	unsigned int CreateTriangleFan(int index, std::vector<unsigned int>& indices,
		int lodCore, int lodLeft, int lodRight, int lodTop, int lodBottom, int x, int z);
	int Powi(int base, int exp);
};
