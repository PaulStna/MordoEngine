#include "Geomipmapping.h"
#include "../Core/Managers/Manager.h"
#include "../Core/Texture/Texture.h"
#include "../Core/Shader/Shader.h"

Geomipmapping::Geomipmapping(const terrain::Terrain& terrain, int patchSize) : m_PatchSize(patchSize)
{
	SetTextureScale(50.0f);
	SetHeightThresholds(0.3f, 0.7f);
	CreateGeomipGrid(terrain);
	m_Texture1ID = "grass";
	m_Texture2ID = "dirt";
	m_Texture3ID = "rock";
}

void Geomipmapping::CreateGeomipGrid(const terrain::Terrain& terrain)
{
	m_Width = terrain.GetSize();
	m_Depth = terrain.GetSize();

	if ((m_Width - 1) % (m_PatchSize - 1) != 0) {
		int recommendedWidth = ((m_Width - 1 + m_PatchSize - 1) / (m_PatchSize - 1)) * (m_PatchSize - 1) + 1;
		printf("Error: Width minus 1 (%d) must be divisible by PatchSize minus 1 (%d)\n",
			m_Width - 1, m_PatchSize - 1);
		printf("Try using Width = %d\n", recommendedWidth);
		exit(-1);
	}

	if ((m_Depth - 1) % (m_PatchSize - 1) != 0) {
		int recommendedDepth = ((m_Depth - 1 + m_PatchSize - 1) / (m_PatchSize - 1)) * (m_PatchSize - 1) + 1;
		printf("Error: Depth minus 1 (%d) must be divisible by PatchSize minus 1 (%d)\n",
			m_Depth - 1, m_PatchSize - 1);
		printf("Try using Depth = %d\n", recommendedDepth);
		exit(-1);
	}

	if (m_PatchSize < 3) {
		printf("Error: The minimum patch size is 3 (given: %d)\n", m_PatchSize);
		exit(-1);
	}

	if (m_PatchSize % 2 == 0) {
		printf("Error: Patch size must be an odd number (given: %d)\n", m_PatchSize);
		exit(-1);
	}

	CreateGLState();
	PopulateBuffers(terrain);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Geomipmapping::CreateGLState()
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

void Geomipmapping::PopulateBuffers(const terrain::Terrain& terrain)
{
	std::vector<terrain::Vertex> vertices;
	vertices.resize(terrain.GetSize() * terrain.GetSize());

	std::vector<unsigned int> indices;
	int numQuads = (m_PatchSize - 1) * (m_PatchSize - 1);
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

void Geomipmapping::InitVertices(const terrain::Terrain& terrain, std::vector<terrain::Vertex>& vertices)
{
	int index = 0;
	for (int x = 0; x < m_Width; x++) {
		for (int z = 0; z < m_Depth; z++) {
			assert(index < vertices.size());
			vertices[index++].InitVertex(terrain, x, z);
		}
	}
}

void Geomipmapping::InitIndices(std::vector<unsigned int>& indices)
{
	int index = 0;

	for (int z = 0; z < m_PatchSize - 1; z += 2) {
		for (int x = 0; x < m_PatchSize - 1; x += 2) {
			unsigned int indexCenter = (z + 1) * m_Width + x + 1;

			unsigned int indexTemp1 = z * m_Width + x;
			unsigned int indexTemp2 = (z + 1) * m_Width + x;

			index = AddTriangle(index, indices, indexCenter, indexTemp1, indexTemp2);

			indexTemp1 = indexTemp2;
			indexTemp2 += m_Width;
			index = AddTriangle(index, indices, indexCenter, indexTemp1, indexTemp2);

			indexTemp1 = indexTemp2;
			indexTemp2++;
			index = AddTriangle(index, indices, indexCenter, indexTemp1, indexTemp2);

			indexTemp1 = indexTemp2;
			indexTemp2++;
			index = AddTriangle(index, indices, indexCenter, indexTemp1, indexTemp2);

			indexTemp1 = indexTemp2;
			indexTemp2 -= m_Width;
			index = AddTriangle(index, indices, indexCenter, indexTemp1, indexTemp2);

			indexTemp1 = indexTemp2;
			indexTemp2 -= m_Width;
			index = AddTriangle(index, indices, indexCenter, indexTemp1, indexTemp2);

			indexTemp1 = indexTemp2;
			indexTemp2--;
			index = AddTriangle(index, indices, indexCenter, indexTemp1, indexTemp2);

			indexTemp1 = indexTemp2;
			indexTemp2--;
			index = AddTriangle(index, indices, indexCenter, indexTemp1, indexTemp2);
		}
	}

	assert(index == indices.size());
}

void Geomipmapping::Render(const Shader& shader)
{
	shader.SetInt("texture1", 0);
	glActiveTexture(GL_TEXTURE0);
	Manager<Texture>::Get(m_Texture1ID).Use();

	shader.SetInt("texture2", 1);
	glActiveTexture(GL_TEXTURE1);
	Manager<Texture>::Get(m_Texture2ID).Use();

	shader.SetInt("texture3", 2);
	glActiveTexture(GL_TEXTURE2);
	Manager<Texture>::Get(m_Texture3ID).Use();

	shader.SetFloat("textureScale", m_TextureScale);
	shader.SetFloat("heightThreshold1", m_HeightThreshold1);
	shader.SetFloat("heightThreshold2", m_HeightThreshold2);

	glBindVertexArray(m_Vao);
	for (int z = 0; z < m_Depth - 1; z += (m_PatchSize - 1)) {
		for (int x = 0; x < m_Width - 1; x += (m_PatchSize - 1)) {
			int baseVertex = z * m_Width + x;
			int numIndices = (m_PatchSize - 1) * (m_PatchSize - 1) * 6;

			glDrawElementsBaseVertex(GL_TRIANGLES, numIndices,
				GL_UNSIGNED_INT, nullptr, baseVertex);
		}
	}
	glBindVertexArray(0);
}

void Geomipmapping::CalculateSmoothNormals(std::vector<terrain::Vertex>& vertices, std::vector<unsigned int>& indices) 
{
	for (int z = 0; z < m_Depth - 1; z += (m_PatchSize - 1)) {
		for (int x = 0; x < m_Width - 1; x += (m_PatchSize - 1)) {
			int baseVertex = z * m_Width + x;

			for (unsigned int i = 0; i < indices.size(); i += 3) {
				unsigned int index0 = baseVertex + indices[i];
				unsigned int index1 = baseVertex + indices[i + 1];
				unsigned int index2 = baseVertex + indices[i + 2];

				glm::vec3 v1 = vertices[index1].pos - vertices[index0].pos;
				glm::vec3 v2 = vertices[index2].pos - vertices[index0].pos;
				glm::vec3 triangleNormal = glm::normalize(glm::cross(v1, v2));

				vertices[index0].normal += triangleNormal;
				vertices[index1].normal += triangleNormal;
				vertices[index2].normal += triangleNormal;
			}
		}
	}

	for (auto& vertex : vertices) {
		vertex.normal = glm::normalize(vertex.normal);
	}
}

unsigned int Geomipmapping::AddTriangle(unsigned index, std::vector<unsigned>& indices, unsigned v1, unsigned v2, unsigned v3)
{
	assert(index < indices.size());
	indices[index++] = v1;
	assert(index < indices.size());
	indices[index++] = v2;
	assert(index < indices.size());
	indices[index++] = v3;

	return index;
}


void Geomipmapping::SetTextureScale(float scale)
{
	m_TextureScale = scale;
}

void Geomipmapping::SetHeightThresholds(float threshold1, float threshold2) {
	m_HeightThreshold1 = threshold1;
	m_HeightThreshold2 = threshold2;
}

Geomipmapping::~Geomipmapping()
{
	glDeleteBuffers(1, &m_Vbo);
	glDeleteBuffers(1, &m_Ebo);
	glDeleteVertexArrays(1, &m_Vao);
}
