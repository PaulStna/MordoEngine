#include "Geomipmapping.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <glm/geometric.hpp>

#define ENABLE_TERRAIN_DEBUG 1

#if ENABLE_TERRAIN_DEBUG
#include <chrono>
#endif

Geomipmapping::Geomipmapping(const Terrain& terrain, int patchSize)
{
	m_PatchSize = patchSize;
	Build(terrain);
}

void Geomipmapping::Build(const Terrain& terrain)
{
	ValidateGrid(terrain);

	m_Vertices.assign(static_cast<std::size_t>(m_Width) * m_Depth, TerrainVertex{});
	InitVertices(terrain);

	m_Indices.assign(CalcNumIndices(), 0u);
	const int numIndices = InitIndices();
	m_Indices.resize(numIndices);

	CalculateSmoothNormals();

	m_DrawCalls.clear();
	m_DrawCalls.reserve(static_cast<std::size_t>(m_NumPatchesX) * m_NumPatchesZ);
}

void Geomipmapping::ValidateGrid(const Terrain& terrain)
{
	m_Width = terrain.GetSize();
	m_Depth = terrain.GetSize();

	if ((m_Width - 1) % (m_PatchSize - 1) != 0) {
		int recommendedWidth = ((m_Width - 1 + m_PatchSize - 1) / (m_PatchSize - 1)) * (m_PatchSize - 1) + 1;
		printf("Error: Width minus 1 (%d) must be divisible by PatchSize minus 1 (%d)\n",
			m_Width - 1, m_PatchSize - 1);
		printf("Try using Width = %d\n", recommendedWidth);
		exit(0);
	}

	if ((m_Depth - 1) % (m_PatchSize - 1) != 0) {
		int recommendedDepth = ((m_Depth - 1 + m_PatchSize - 1) / (m_PatchSize - 1)) * (m_PatchSize - 1) + 1;
		printf("Error: Depth minus 1 (%d) must be divisible by PatchSize minus 1 (%d)\n",
			m_Depth - 1, m_PatchSize - 1);
		printf("Try using Depth = %d\n", recommendedDepth);
		exit(0);
	}

	if (m_PatchSize < 3) {
		printf("Error: The minimum patch size is 3 (given: %d)\n", m_PatchSize);
		exit(0);
	}

	if (m_PatchSize % 2 == 0) {
		printf("Error: Patch size must be an odd number (given: %d)\n", m_PatchSize);
		exit(0);
	}

	m_NumPatchesX = (m_Width - 1) / (m_PatchSize - 1);
	m_NumPatchesZ = (m_Depth - 1) / (m_PatchSize - 1);

	float worldScale = terrain.GetWorldScale();
	m_MaxLOD = m_LodManager.InitLodManager(m_PatchSize, m_NumPatchesX, m_NumPatchesZ, worldScale);
	m_LodInfo.resize(m_MaxLOD + 1);
}

void Geomipmapping::InitVertex(TerrainVertex& vertex, const Terrain& terrain,
	std::size_t x, std::size_t z) const
{
	vertex.pos = glm::vec3{
		static_cast<float>(x) * terrain.GetWorldScale(),
		terrain.GetScaledHeightAt(x, z),
		static_cast<float>(z) * terrain.GetWorldScale(),
	};

	std::size_t terrainSize = terrain.GetSize();
	vertex.texCoord = glm::vec2{
		static_cast<float>(x) / static_cast<float>(terrainSize - 1),
		static_cast<float>(z) / static_cast<float>(terrainSize - 1)
	};

	vertex.height = terrain.GetNormalizedHeightAt(x, z);
}

void Geomipmapping::InitVertices(const Terrain& terrain)
{
	int index = 0;

	for (int z = 0; z < m_Depth; z++) {
		for (int x = 0; x < m_Width; x++) {
			assert(index < m_Vertices.size());
			InitVertex(m_Vertices[index], terrain, x, z);
			index++;
		}
	}

	assert(index == m_Vertices.size());
}

int Geomipmapping::CalcNumIndices()
{
	int numQuads = (m_PatchSize - 1) * (m_PatchSize - 1);
	int numIndices = 0;
	int maxPermutationsPerLevel = 16;
	const int indicesPerQuad = 6;

	for (int lod = 0; lod <= m_MaxLOD; lod++) {
		numIndices += numQuads * indicesPerQuad * maxPermutationsPerLevel;
		numQuads /= 4;
	}

	return numIndices;
}

int Geomipmapping::InitIndices()
{
	int index = 0;

	for (int lod = 0; lod <= m_MaxLOD; lod++) {
		index = InitIndicesLOD(index, lod);
	}

	return index;
}

int Geomipmapping::InitIndicesLOD(int index, int lod)
{
	int totalIndicesForLOD = 0;

	for (int l = 0; l < 2; l++) {
		for (int r = 0; r < 2; r++) {
			for (int t = 0; t < 2; t++) {
				for (int b = 0; b < 2; b++) {
					m_LodInfo[lod].info[l][r][t][b].start = index;
					index = InitIndicesLODSingle(index, lod, lod + l, lod + r, lod + t, lod + b);

					m_LodInfo[lod].info[l][r][t][b].count = index - m_LodInfo[lod].info[l][r][t][b].start;
					totalIndicesForLOD += m_LodInfo[lod].info[l][r][t][b].count;
				}
			}
		}
	}

	return index;
}

int Geomipmapping::InitIndicesLODSingle(int index, int lodCore, int lodLeft, int lodRight, int lodTop, int lodBottom)
{
	int fanStep = Powi(2, lodCore + 1);
	int endPos = m_PatchSize - 1 - fanStep;

	for (int z = 0; z <= endPos; z += fanStep) {
		for (int x = 0; x <= endPos; x += fanStep) {
			int lLeft = x == 0 ? lodLeft : lodCore;
			int lRight = x == endPos ? lodRight : lodCore;
			int lBottom = z == 0 ? lodBottom : lodCore;
			int lTop = z == endPos ? lodTop : lodCore;

			index = CreateTriangleFan(index, lodCore, lLeft, lRight, lTop, lBottom, x, z);
		}
	}

	return index;
}

unsigned int Geomipmapping::CreateTriangleFan(int index, int lodCore, int lodLeft, int lodRight, int lodTop, int lodBottom, int x, int z)
{
	int stepLeft = Powi(2, lodLeft);
	int stepRight = Powi(2, lodRight);
	int stepTop = Powi(2, lodTop);
	int stepBottom = Powi(2, lodBottom);
	int stepCenter = Powi(2, lodCore);

	unsigned int indexCenter = (z + stepCenter) * m_Width + x + stepCenter;

	unsigned int indexTemp1 = z * m_Width + x;
	unsigned int indexTemp2 = (z + stepLeft) * m_Width + x;

	index = AddTriangle(index, indexCenter, indexTemp1, indexTemp2);

	if (lodLeft == lodCore) {
		indexTemp1 = indexTemp2;
		indexTemp2 += stepLeft * m_Width;

		index = AddTriangle(index, indexCenter, indexTemp1, indexTemp2);
	}

	indexTemp1 = indexTemp2;
	indexTemp2 += stepTop;

	index = AddTriangle(index, indexCenter, indexTemp1, indexTemp2);

	if (lodTop == lodCore) {
		indexTemp1 = indexTemp2;
		indexTemp2 += stepTop;

		index = AddTriangle(index, indexCenter, indexTemp1, indexTemp2);
	}

	indexTemp1 = indexTemp2;
	indexTemp2 -= stepRight * m_Width;

	index = AddTriangle(index, indexCenter, indexTemp1, indexTemp2);

	if (lodRight == lodCore) {
		indexTemp1 = indexTemp2;
		indexTemp2 -= stepRight * m_Width;

		index = AddTriangle(index, indexCenter, indexTemp1, indexTemp2);
	}

	indexTemp1 = indexTemp2;
	indexTemp2 -= stepBottom;

	index = AddTriangle(index, indexCenter, indexTemp1, indexTemp2);

	if (lodBottom == lodCore) {
		indexTemp1 = indexTemp2;
		indexTemp2 -= stepBottom;

		index = AddTriangle(index, indexCenter, indexTemp1, indexTemp2);
	}

	return index;
}

int Geomipmapping::Powi(int base, int exp)
{
	return 1 << exp;
}

unsigned int Geomipmapping::AddTriangle(unsigned int index,
	unsigned int vertex1, unsigned int vertex2, unsigned int vertex3)
{
	assert(index < m_Indices.size());
	m_Indices[index++] = vertex1;
	assert(index < m_Indices.size());
	m_Indices[index++] = vertex2;
	assert(index < m_Indices.size());
	m_Indices[index++] = vertex3;

	return index;
}

void Geomipmapping::CalculateSmoothNormals()
{
	for (int z = 0; z < m_Depth - 1; z += (m_PatchSize - 1)) {
		for (int x = 0; x < m_Width - 1; x += (m_PatchSize - 1)) {
			int baseVertex = z * m_Width + x;
			int numIndices = m_LodInfo[0].info[0][0][0][0].count;

			for (int i = 0; i < numIndices; i += 3) {
				unsigned int index0 = baseVertex + m_Indices[i];
				unsigned int index1 = baseVertex + m_Indices[i + 1];
				unsigned int index2 = baseVertex + m_Indices[i + 2];

				glm::vec3 edge1 = m_Vertices[index1].pos - m_Vertices[index0].pos;
				glm::vec3 edge2 = m_Vertices[index2].pos - m_Vertices[index0].pos;

				glm::vec3 triangleNormal = glm::cross(edge1, edge2);
				triangleNormal = glm::normalize(triangleNormal);

				m_Vertices[index0].normal += triangleNormal;
				m_Vertices[index1].normal += triangleNormal;
				m_Vertices[index2].normal += triangleNormal;
			}
		}
	}

	for (unsigned int i = 0; i < m_Vertices.size(); i++) {
		m_Vertices[i].normal = glm::normalize(m_Vertices[i].normal);
	}
}

void Geomipmapping::SelectLods(const glm::vec3& cameraPos)
{
	m_LodManager.Update(cameraPos);

	m_DrawCalls.clear();

	for (int patchZ = 0; patchZ < m_NumPatchesZ; patchZ++) {
		for (int patchX = 0; patchX < m_NumPatchesX; patchX++) {
			const LodManager::PatchLod& patchLod = m_LodManager.GetPatchLod(patchX, patchZ);
			int c = patchLod.core;
			int l = patchLod.left;
			int r = patchLod.right;
			int t = patchLod.top;
			int b = patchLod.bottom;

			const SingleLodInfo& lodInfo = m_LodInfo[c].info[l][r][t][b];

			int z = patchZ * (m_PatchSize - 1);
			int x = patchX * (m_PatchSize - 1);

			MeshDrawCall drawCall;
			drawCall.indexOffset = lodInfo.start;
			drawCall.indexCount = lodInfo.count;
			drawCall.baseVertex = z * m_Width + x;

			m_DrawCalls.push_back(drawCall);
		}
	}
}

const std::vector<int>& Geomipmapping::RebuildModifiedVertices(const Terrain& terrain)
{
	m_AffectedIndices.clear();

	if (!terrain.HasModifications()) {
		return m_AffectedIndices;
	}

	const std::vector<int>& modifiedIndices = terrain.GetModifiedVertices();

	if (modifiedIndices.empty()) {
		return m_AffectedIndices;
	}

#if ENABLE_TERRAIN_DEBUG
	auto t1 = std::chrono::high_resolution_clock::now();
#endif

	// Moving a vertex changes its neighbours' normals too, so widen the set by one ring.
	std::vector<bool> affectedMask(static_cast<std::size_t>(m_Width) * m_Depth, false);
	m_AffectedIndices.reserve(modifiedIndices.size() * 9);

	for (int index : modifiedIndices) {
		int x = index % m_Width;
		int z = index / m_Width;

		for (int dz = -1; dz <= 1; dz++) {
			for (int dx = -1; dx <= 1; dx++) {
				int nx = x + dx;
				int nz = z + dz;

				if (nx >= 0 && nx < m_Width && nz >= 0 && nz < m_Depth) {
					int nindex = nz * m_Width + nx;
					if (!affectedMask[nindex]) {
						affectedMask[nindex] = true;
						m_AffectedIndices.push_back(nindex);
					}
				}
			}
		}
	}

#if ENABLE_TERRAIN_DEBUG
	auto t2 = std::chrono::high_resolution_clock::now();
#endif

	for (int index : m_AffectedIndices) {
		int x = index % m_Width;
		int z = index / m_Width;
		InitVertex(m_Vertices[index], terrain, x, z);
	}

#if ENABLE_TERRAIN_DEBUG
	auto t3 = std::chrono::high_resolution_clock::now();
#endif

	for (int index : m_AffectedIndices) {
		RecalculateNormalAt(index);
	}

#if ENABLE_TERRAIN_DEBUG
	auto t4 = std::chrono::high_resolution_clock::now();

	printf("=== RebuildModifiedVertices Timing ===\n");
	printf("Modified vertices: %zu\n", modifiedIndices.size());
	printf("Affected vertices: %zu\n", m_AffectedIndices.size());
	printf("Expand neighbors: %.2f ms\n", std::chrono::duration<double, std::milli>(t2 - t1).count());
	printf("InitVertex: %.2f ms\n", std::chrono::duration<double, std::milli>(t3 - t2).count());
	printf("Recalc normals: %.2f ms\n", std::chrono::duration<double, std::milli>(t4 - t3).count());
	printf("TOTAL: %.2f ms\n\n", std::chrono::duration<double, std::milli>(t4 - t1).count());
#endif

	return m_AffectedIndices;
}

void Geomipmapping::RecalculateNormalAt(int index)
{
	int x = index % m_Width;
	int z = index / m_Width;

	glm::vec3 normal(0.0f);
	glm::vec3 current = m_Vertices[index].pos;

	if (x > 0) {
		glm::vec3 left = m_Vertices[z * m_Width + (x - 1)].pos;

		if (z < m_Depth - 1) {
			glm::vec3 up = m_Vertices[(z + 1) * m_Width + x].pos;
			glm::vec3 upLeft = m_Vertices[(z + 1) * m_Width + (x - 1)].pos;

			normal += glm::cross(left - current, upLeft - current);
			normal += glm::cross(upLeft - current, up - current);
		}

		if (z > 0) {
			glm::vec3 down = m_Vertices[(z - 1) * m_Width + x].pos;
			glm::vec3 downLeft = m_Vertices[(z - 1) * m_Width + (x - 1)].pos;

			normal += glm::cross(downLeft - current, left - current);
			normal += glm::cross(down - current, downLeft - current);
		}
	}

	if (x < m_Width - 1) {
		glm::vec3 right = m_Vertices[z * m_Width + (x + 1)].pos;

		if (z < m_Depth - 1) {
			glm::vec3 up = m_Vertices[(z + 1) * m_Width + x].pos;
			glm::vec3 upRight = m_Vertices[(z + 1) * m_Width + (x + 1)].pos;

			normal += glm::cross(up - current, upRight - current);
			normal += glm::cross(upRight - current, right - current);
		}

		if (z > 0) {
			glm::vec3 down = m_Vertices[(z - 1) * m_Width + x].pos;
			glm::vec3 downRight = m_Vertices[(z - 1) * m_Width + (x + 1)].pos;

			normal += glm::cross(right - current, downRight - current);
			normal += glm::cross(downRight - current, down - current);
		}
	}

	if (glm::length(normal) > 0.0f) {
		m_Vertices[index].normal = glm::normalize(normal);
	}
	else {
		m_Vertices[index].normal = glm::vec3(0.0f, 1.0f, 0.0f);
	}
}
