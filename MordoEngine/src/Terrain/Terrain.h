#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <cstddef>
#include <limits>

/// A heightmap and the operations that read and edit it. Knows nothing about
/// how it gets drawn.
///
/// Heights are stored once, in raw sample units, and exposed three ways:
/// GetHeightAt is the stored value, GetNormalizedHeightAt is that mapped to
/// 0..1, and GetScaledHeightAt is what the world actually uses. Keeping one
/// copy and converting on read is what stops the three from drifting apart.
///
/// The base of the generator hierarchy: loading a file, faulting and midpoint
/// displacement all derive from this and differ only in how they fill the same
/// array.
///
/// Edits are tracked rather than applied blindly, so a brush stroke can
/// re-upload the few hundred vertices it touched instead of the whole mesh.
class Terrain
{
public:
	/// Largest value a raw uint16 heightmap sample can hold.
	static constexpr float RAW_HEIGHT_MAX =
		static_cast<float>(std::numeric_limits<std::uint16_t>::max());

	Terrain() = default;

	/// Reads a raw uint16 heightmap. The file carries no dimensions, so the side
	/// is taken to be the square root of its length.
	/// @return false if the file cannot be read or is not a square.
	bool LoadHeightMap(const std::string& filename);

	/// Writes the heightmap back in the same raw uint16 form it was read in.
	bool SaveHeightMap(const std::string& filename) const;

	bool UnloadHeightMap() noexcept;

	/// How far a full-range sample reaches in world units.
	float GetHeightScale() const noexcept;
	void SetHeightScale(float scale) noexcept;

	/// @param x Grid index, not a world coordinate.
	/// @param z Grid index, same.
	/// @return The stored sample, in raw units.
	float GetHeightAt(std::size_t x, std::size_t z) const;
	void SetHeightAt(float height, std::size_t x, std::size_t z);

	/// The same sample mapped to 0..1 across the terrain's range, which is what
	/// texture splatting blends on.
	float GetNormalizedHeightAt(std::size_t x, std::size_t z) const;

	/// The same sample in world units, height scale applied. This is the one
	/// anything standing on the ground wants.
	float GetScaledHeightAt(std::size_t x, std::size_t z) const;

	/// Bilinearly samples between the four surrounding vertices, for anything
	/// that moves continuously over the terrain rather than snapping to grid
	/// points.
	/// @param x World-space X, not a grid index. Clamped to the terrain.
	/// @param z World-space Z, same.
	float GetHeightInterpolated(float x, float z) const;

	/// Side length in vertices, so the grid holds this squared.
	std::size_t GetSize() const noexcept;

	/// World units between two neighbouring grid posts.
	int GetWorldScale() const noexcept;
	void SetWorldScale(int worldScale) noexcept;

	/// Records that a vertex changed, so only what moved is re-uploaded.
	/// @param x Grid index.
	/// @param z Grid index.
	void MarkVertexAsModified(int x, int z);

	/// Drops the record once the changes have been uploaded.
	void ClearModifications();

	/// Flat vertex indices, in the order the mesh stores them.
	const std::vector<int>& GetModifiedVertices() const { return m_ModifiedVertexIndices; }
	bool HasModifications() const { return m_HasModifications; }

	virtual ~Terrain() = default;

protected:
	/// The grid itself: one float per post, square, indexed row by row.
	struct HeightData
	{
		std::vector<float> data;
		std::size_t size = 0;
	};

	HeightData p_HeightData;
	int p_WorldScale = 2;
	float p_HeightScale = 1.0f;
	float p_MinHeight = 0.0f;
	float p_MaxHeight = RAW_HEIGHT_MAX;
	unsigned int p_Width = 0;
	unsigned int p_Depth = 0;

	// Whether the stored samples have already been mapped into the min/max
	// range, so rescaling twice cannot happen.
	bool p_IsScaled = false;

	/// Allocates the grid and records its range. Every generator calls this
	/// before filling p_HeightData.
	void Initialize(std::size_t size, int worldScale, float minH, float maxH);

	/// Maps the generated values into the given range, which is what makes the
	/// output of different generators comparable.
	void RescaleData(float min, float max);

private:
	std::vector<int> m_ModifiedVertexIndices;
	bool m_HasModifications = false;
	std::size_t index(std::size_t x, std::size_t z) const noexcept;
};
