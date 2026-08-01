#pragma once

#include <glm/vec3.hpp>
#include <vector>

/// Decides which level of detail each terrain patch is drawn at this frame, and
/// how its four edges have to be stitched to its neighbours.
///
/// The core LOD comes from the distance to the camera. On its own that cracks
/// the terrain: two neighbouring patches at different levels put a different
/// number of vertices along their shared border, and the gap shows as a seam of
/// sky. So each patch also carries a LOD per edge, resolved against whoever is
/// next to it, and the mesh emits a matching border for that edge.
///
/// This is the technique half of the terrain, and it makes no GL calls: it only
/// decides which index ranges the renderer should draw.
class LodManager
{
public:
	/// What one patch is drawn at: its interior, plus the four borders that have
	/// to agree with the patches around it.
	struct PatchLod
	{
		int core = 0;
		int left = 0;
		int right = 0;
		int top = 0;
		int bottom = 0;
	};

	/// @param patchSize   Vertices along one edge of a patch, including the
	///                    shared border.
	/// @param worldScale  World units between two grid posts, needed to turn a
	///                    camera distance into a level.
	/// @return The highest LOD index this configuration supports, which is
	///         fixed by how many times a patch can be halved.
	int  InitLodManager(int patchSize, int numPatchesX, int numPatchesZ, float worldScale);

	/// Recomputes every patch's level for this frame.
	///
	/// Two passes, and both are needed: the first picks each core LOD from its
	/// distance, and the second reconciles the edges, which can only be done
	/// once every neighbour's core is known.
	///
	/// @param cameraPos World space.
	void Update(const glm::vec3& cameraPos);

	/// @param patchX Patch index, not a vertex index.
	/// @param patchZ Patch index, same.
	const PatchLod& GetPatchLod(int patchX, int patchZ) const;

private:
	void CalcMaxLOD();
	void CalcLodRegions();

	void UpdateLodMapPass1(const glm::vec3& cameraPos);
	void UpdateLodMapPass2();

	int  DistanceToLod(float distance) const;

	inline int Index(int x, int z) const
	{
		return z * m_NumPatchesX + x;
	}

	int   m_MaxLOD = 0;
	int   m_PatchSize = 0;
	int   m_NumPatchesX = 0;
	int   m_NumPatchesZ = 0;
	float m_WorldScale = 1.0f;

	// One entry per patch, rebuilt every Update.
	std::vector<PatchLod> m_Map;

	// Cumulative distance threshold per LOD, computed once at init, so choosing
	// a level per frame is a walk over this rather than a recomputation.
	std::vector<int>      m_Regions;
};
