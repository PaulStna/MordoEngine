#include "TerrainMeshRenderer.h"

TerrainMeshRenderer::TerrainMeshRenderer()
{
	CreateGLState();

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void TerrainMeshRenderer::CreateGLState()
{
	glGenVertexArrays(1, &m_Vao);
	glBindVertexArray(m_Vao);

	glGenBuffers(1, &m_Vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);

	glGenBuffers(1, &m_Ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ebo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex),
		(const void*)offsetof(TerrainVertex, pos));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex),
		(const void*)offsetof(TerrainVertex, texCoord));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex),
		(const void*)offsetof(TerrainVertex, normal));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex),
		(const void*)offsetof(TerrainVertex, height));
}

void TerrainMeshRenderer::Upload(const std::vector<TerrainVertex>& vertices,
	const std::vector<unsigned int>& indices)
{
	if (vertices.empty() || indices.empty()) {
		return;
	}

	glBindVertexArray(m_Vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(),
		vertices.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(),
		indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void TerrainMeshRenderer::UpdateVertices(const std::vector<TerrainVertex>& vertices,
	const std::vector<int>& modifiedIndices)
{
	if (modifiedIndices.empty()) {
		return;
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);

	for (int index : modifiedIndices) {
		glBufferSubData(GL_ARRAY_BUFFER,
			index * sizeof(TerrainVertex),
			sizeof(TerrainVertex),
			&vertices[index]);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void TerrainMeshRenderer::Render(const std::vector<MeshDrawCall>& drawCalls) const
{
	glBindVertexArray(m_Vao);

	for (const MeshDrawCall& drawCall : drawCalls) {
		const std::size_t byteOffset = sizeof(unsigned int) * drawCall.indexOffset;

		glDrawElementsBaseVertex(GL_TRIANGLES, drawCall.indexCount,
			GL_UNSIGNED_INT, (void*)byteOffset, drawCall.baseVertex);
	}

	glBindVertexArray(0);
}

TerrainMeshRenderer::~TerrainMeshRenderer()
{
	glDeleteBuffers(1, &m_Vbo);
	glDeleteBuffers(1, &m_Ebo);
	glDeleteVertexArrays(1, &m_Vao);
}
