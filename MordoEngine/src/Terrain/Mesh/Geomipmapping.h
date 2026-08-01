#pragma once
#include "LodManager.h"
#include "../Terrain.h"
#include "../TerrainVertex.h"
#include "../../Renderer/MeshDrawCall.h"
#include <glm/vec3.hpp>
#include <vector>

/// Turns a heightmap into drawable geometry: one vertex per grid post, and an
/// index buffer holding every LOD of every patch at once.
///
/// The whole point is that nothing is rebuilt per frame. Every combination a
/// patch could need is generated up front, so choosing a level of detail is
/// choosing which slice of the index buffer to draw. SelectLods walks the
/// patches and fills the draw call list; the vertices never move.
///
/// Makes no GL calls: it produces vertices, indices and draw calls, and the
/// renderer is what uploads them.
class Geomipmapping
{
public:
	/// @param patchSize Vertices along one patch edge, shared border included.
	///                  Must be a power of two plus one, since a patch is
	///                  halved once per level.
	Geomipmapping(const Terrain& terrain, int patchSize);

	/// Builds every vertex, every index and the LOD tables. Call once.
	void Build(const Terrain& terrain);

	/// Refreshes only the vertices the terrain recorded as edited, and the
	/// normals around them, since a moved post changes its neighbours' normals.
	/// @return Indices of every vertex that actually changed, for the renderer
	///         to re-upload. Backed by a member, so it is valid until the next
	///         call.
	const std::vector<int>& RebuildModifiedVertices(const Terrain& terrain);

	const std::vector<TerrainVertex>& GetVertices() const { return m_Vertices; }
	const std::vector<unsigned int>&  GetIndices()  const { return m_Indices; }

	/// Picks a level for every patch and stitches the edges, then rebuilds the
	/// draw call list. Once per frame, before rendering.
	void SelectLods(const glm::vec3& cameraPos);

	/// One entry per visible patch, valid until the next SelectLods.
	const std::vector<MeshDrawCall>& GetDrawCalls() const { return m_DrawCalls; }

private:
	/// A slice of the shared index buffer.
	struct SingleLodInfo
	{
		int start = 0;
		int count = 0;
	};

	/// Every index range one LOD of a patch might need.
	///
	/// The four dimensions are the four edges, each either matching this
	/// patch's level or stepping down to a coarser neighbour. Pre-generating
	/// all sixteen is what makes stitching free at draw time: the edge
	/// combination is a lookup, not a mesh rebuild.
	struct LodInfo
	{
		SingleLodInfo info[2][2][2][2];
	};

	int m_Width = 0;
	int m_Depth = 0;
	int m_PatchSize = 0;
	int m_NumPatchesX = 0;
	int m_NumPatchesZ = 0;
	int m_MaxLOD = 0;

	LodManager m_LodManager;
	std::vector<LodInfo> m_LodInfo;

	std::vector<TerrainVertex> m_Vertices;
	std::vector<unsigned int>  m_Indices;
	std::vector<MeshDrawCall>  m_DrawCalls;

	// Reused across modification updates so no allocation happens per edit.
	std::vector<int> m_AffectedIndices;

	void ValidateGrid(const Terrain& terrain);
	void InitVertex(TerrainVertex& vertex, const Terrain& terrain,
		std::size_t x, std::size_t z) const;
	void InitVertices(const Terrain& terrain);
	int  InitIndices();
	void CalculateSmoothNormals();
	void RecalculateNormalAt(int index);
	unsigned int AddTriangle(unsigned int index, unsigned int vertex1,
		unsigned int vertex2, unsigned int vertex3);
	int CalcNumIndices();
	int InitIndicesLOD(int index, int lod);
	int InitIndicesLODSingle(int index, int lodCore, int lodLeft, int lodRight,
		int lodTop, int lodBottom);
	unsigned int CreateTriangleFan(int index, int lodCore, int lodLeft, int lodRight,
		int lodTop, int lodBottom, int x, int z);
	static int Powi(int base, int exp);
};
