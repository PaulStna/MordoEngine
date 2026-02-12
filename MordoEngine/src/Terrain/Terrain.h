#pragma once
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <string>
#include <vector>
#include <cstdint>
#include <cstddef>
#include <limits>

namespace terrain
{

	struct HeightData
	{
		std::vector<float> data;
		std::size_t size = 0;
	};

	constexpr float RAW_HEIGHT_MAX =
		static_cast<float>(std::numeric_limits<std::uint16_t>::max());

	class Terrain
	{
	public:
		Terrain() = default;

		bool LoadHeightMap(const std::string& filename);
		bool SaveHeightMap(const std::string& filename) const;
		bool UnloadHeightMap() noexcept;

		float GetHeightScale() const noexcept;
		void SetHeightScale(float scale) noexcept;

		float GetHeightAt(std::size_t x, std::size_t z) const;
		void SetHeightAt(float height, std::size_t x, std::size_t z);

		float GetNormalizedHeightAt(std::size_t x, std::size_t z) const;
		float GetScaledHeightAt(std::size_t x, std::size_t z) const;

		float GetHeightInterpolated(float x, float z) const;

		std::size_t GetSize() const noexcept;
		int GetWorldScale() const noexcept;

		virtual ~Terrain() = default;

	protected:
		HeightData p_HeightData;
		int p_WorldScale = 1.0f;
		float p_HeightScale = 1.0f;
		float p_MinHeight = 0.0f;
		float p_MaxHeight = RAW_HEIGHT_MAX;
		unsigned int p_Width = 0;
		unsigned int p_Depth = 0;
		bool p_IsScaled = false;

		void Initialize(std::size_t size, int worldScale, float minH, float maxH);
		void RescaleData(float min, float max);

	private:
		std::size_t index(std::size_t x, std::size_t z) const noexcept;
	};

	struct Vertex
	{
		glm::vec3 pos;
		glm::vec2 texCoord;
		glm::vec3 normal = glm::vec3(0.f);
		float height;
		void InitVertex(const Terrain& terrain, std::size_t x, std::size_t z);
	};
}