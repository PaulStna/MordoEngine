#include "TriangleRenderer.h"
#include "../Core/Managers/Manager.h"
#include "../Core/Texture/Texture.h"
#include <cassert>

TriangleRenderer::TriangleRenderer(Shader& shader, const terrain::Terrain& terrain) : Renderer(shader)
{
	SetTextureScale(50.0f);
	SetHeightThresholds(0.3f, 0.7f);
	CreateTriangleList(terrain);
	m_Texture1ID = "grass";
	m_Texture2ID = "dirt";
	m_Texture3ID = "rock";
}

void TriangleRenderer::SetTextureScale(float scale)
{
	m_TextureScale = scale;
}

void TriangleRenderer::CreateTriangleList(const terrain::Terrain& terrain)
{
	m_Width = terrain.GetSize();
	m_Depth = terrain.GetSize();

	CreateGLState();
	PopulateBuffers(terrain);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void TriangleRenderer::SetHeightThresholds(float threshold1, float threshold2) {
	m_HeightThreshold1 = threshold1;
	m_HeightThreshold2 = threshold2;
}

void TriangleRenderer::CreateGLState()
{
	glGenVertexArrays(1, &m_Vao);
	glBindVertexArray(m_Vao);

	glGenBuffers(1, &m_Vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
	glGenBuffers(1, &m_Ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ebo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(terrain::Vertex),
		(const void*)offsetof(terrain::Vertex, pos));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(terrain::Vertex),
		(const void*)offsetof(terrain::Vertex, texCoord));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(terrain::Vertex),
		(const void*)offsetof(terrain::Vertex, normal));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(terrain::Vertex),
		(const void*)offsetof(terrain::Vertex, height));
}

void TriangleRenderer::PopulateBuffers(const terrain::Terrain& terrain)
{
	std::vector<terrain::Vertex> vertices;
	vertices.resize(terrain.GetSize() * terrain.GetSize());

	std::vector<unsigned int> indices;
	int numQuads = (m_Width - 1) * (m_Depth - 1);
	indices.resize(numQuads * 6);

	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
	InitVertices(terrain, vertices);
	InitIndices(indices);

	CalculateSmoothNormals(vertices, indices);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(),
		&vertices[0], GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(),
		&indices[0], GL_STATIC_DRAW);
}

void TriangleRenderer::InitVertices(const terrain::Terrain& terrain, std::vector<terrain::Vertex>& vertices)
{
	int index = 0;
	for (int x = 0; x < m_Width; x++) {
		for (int z = 0; z < m_Depth; z++) {
			assert(index < vertices.size());
			vertices[index++].InitVertex(terrain, x, z);
		}
	}
}

void TriangleRenderer::InitIndices(std::vector<unsigned int>& indices)
{
	int index = 0;
	for (int x = 0; x < m_Width - 1; x++) {
		for (int z = 0; z < m_Depth - 1; z++) {
			unsigned int IndexBottomLeft = z * m_Width + x;
			unsigned int IndexTopLeft = (z + 1) * m_Width + x;
			unsigned int IndexTopRight = (z + 1) * m_Width + x + 1;
			unsigned int IndexBottomRight = z * m_Width + x + 1;

			assert(index < indices.size());
			indices[index++] = IndexBottomLeft;
			assert(index < indices.size());
			indices[index++] = IndexTopLeft;
			assert(index < indices.size());
			indices[index++] = IndexTopRight;

			assert(index < indices.size());
			indices[index++] = IndexBottomLeft;
			assert(index < indices.size());
			indices[index++] = IndexTopRight;
			assert(index < indices.size());
			indices[index++] = IndexBottomRight;
		}
	}
}

void TriangleRenderer::Render(const glm::vec3& cameraPos)
{
	p_Shader.Use();

	p_Shader.SetInt("texture1", 0);
	glActiveTexture(GL_TEXTURE0);
	Manager<Texture>::Get(m_Texture1ID).Use();

	p_Shader.SetInt("texture2", 1);
	glActiveTexture(GL_TEXTURE1);
	Manager<Texture>::Get(m_Texture2ID).Use();

	p_Shader.SetInt("texture3", 2);
	glActiveTexture(GL_TEXTURE2);
	Manager<Texture>::Get(m_Texture3ID).Use();

	p_Shader.SetFloat("textureScale", m_TextureScale);
	p_Shader.SetFloat("heightThreshold1", m_HeightThreshold1);
	p_Shader.SetFloat("heightThreshold2", m_HeightThreshold2);

	glBindVertexArray(m_Vao);
	glDrawElements(GL_TRIANGLES, (m_Width - 1) * (m_Depth - 1) * 6, GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
}

void TriangleRenderer::CalculateSmoothNormals(std::vector<terrain::Vertex>& vertices, std::vector<unsigned int>& indices) {
	for (unsigned int i = 0; i < indices.size(); i += 3) {
		unsigned int index0 = indices[i];
		unsigned int index1 = indices[i+1];
		unsigned int index2 = indices[i+2];
		glm::vec3 v1 = vertices[index1].pos - vertices[index0].pos;
		glm::vec3 v2 = vertices[index2].pos - vertices[index0].pos;
		glm::vec3 triangleNormal = glm::normalize(glm::cross(v1, v2));

		vertices[index0].normal += triangleNormal;
		vertices[index1].normal += triangleNormal;
		vertices[index2].normal += triangleNormal;
	}

	for (auto& vertex : vertices) {
		vertex.normal = glm::normalize(vertex.normal);
	}
}

TriangleRenderer::~TriangleRenderer()
{
	glDeleteBuffers(1, &m_Vbo);
	glDeleteBuffers(1, &m_Ebo);
	glDeleteVertexArrays(1, &m_Vao);
}
