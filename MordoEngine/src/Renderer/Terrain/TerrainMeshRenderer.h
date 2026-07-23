#pragma once
#include <glad/glad.h>
#include "../MeshDrawCall.h"
#include "../../Terrain/TerrainVertex.h"
#include <vector>

class TerrainMeshRenderer
{
public:
	TerrainMeshRenderer();
	TerrainMeshRenderer(const TerrainMeshRenderer&) = delete;
	TerrainMeshRenderer& operator=(const TerrainMeshRenderer&) = delete;
	~TerrainMeshRenderer();

	// Full (re)upload of the mesh.
	void Upload(const std::vector<terrain::Vertex>& vertices,
		const std::vector<unsigned int>& indices);

	// Re-uploads just the listed vertices, leaving the rest of the buffer alone.
	void UpdateVertices(const std::vector<terrain::Vertex>& vertices,
		const std::vector<int>& modifiedIndices);

	void Render(const std::vector<MeshDrawCall>& drawCalls) const;

private:
	GLuint m_Vao = 0;
	GLuint m_Vbo = 0;
	GLuint m_Ebo = 0;

	void CreateGLState();
};
