#include "Terrain.h"
#include "../Core/FileSystem/FileSystem.h"
#include <fstream>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <cmath>

using namespace terrain;

void Terrain::Initialize(std::size_t size, int worldScale, float minH, float maxH)
{
	p_HeightData.size = size;
	p_HeightData.data.resize(size * size, 0.0f);
	p_Width = size;
	p_Depth = size;
	p_MinHeight = static_cast<float>(minH);
	p_MaxHeight = static_cast<float>(maxH);
	p_WorldScale = worldScale;
	p_IsScaled = false;
}

void Vertex::InitVertex(const Terrain& terrain, std::size_t x, std::size_t z)
{
	pos = glm::vec3{
		static_cast<float>(x) * terrain.GetWorldScale(),
		terrain.GetScaledHeightAt(x, z),
		static_cast<float>(z) * terrain.GetWorldScale(),
	};

	std::size_t terrainSize = terrain.GetSize();
	texCoord = glm::vec2{
		static_cast<float>(x) / static_cast<float>(terrainSize - 1),
		static_cast<float>(z) / static_cast<float>(terrainSize - 1)
	};

	height = terrain.GetNormalizedHeightAt(x, z);
}

bool Terrain::LoadHeightMap(const std::string& filename)
{
	std::ifstream file(FileSystem::getPath(filename), std::ios::binary);
	if (!file)
		return false;

	file.seekg(0, std::ios::end);
	std::streamsize fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	std::size_t totalElements = fileSize / sizeof(std::uint16_t);
	std::size_t size = static_cast<std::size_t>(std::sqrt(totalElements));

	if (size * size != totalElements) {
		std::cerr << "Not a Square!" << std::endl;
		return false;
	}

	std::vector<std::uint16_t> tempData(totalElements);
	file.read(reinterpret_cast<char*>(tempData.data()), fileSize);

	if (!file.good())
		return false;

	Initialize(size, 1.0f, 0.0f, RAW_HEIGHT_MAX);
	for (std::size_t i = 0; i < totalElements; i++) {
		p_HeightData.data[i] = static_cast<float>(tempData[i]);
	}
	return true;
}

bool Terrain::SaveHeightMap(const std::string& filename) const
{
	if (p_HeightData.data.empty())
		return false;

	std::ofstream file(FileSystem::getPath(filename), std::ios::binary);
	if (!file)
		return false;

	std::vector<std::uint16_t> tempData(p_HeightData.data.size());
	for (std::size_t i = 0; i < p_HeightData.data.size(); i++) {
		float clamped = std::clamp(p_HeightData.data[i], 0.0f, RAW_HEIGHT_MAX);
		tempData[i] = static_cast<std::uint16_t>(clamped);
	}

	file.write(
		reinterpret_cast<const char*>(tempData.data()),
		tempData.size() * sizeof(std::uint16_t)
	);

	return file.good();
}

bool Terrain::UnloadHeightMap() noexcept
{
	p_HeightData.data.clear();
	p_HeightData.data.shrink_to_fit();
	p_HeightData.size = 0;
	p_Width = 0;
	p_Depth = 0;
	p_IsScaled = false;
	p_MinHeight = 0.0f;
	p_MaxHeight = RAW_HEIGHT_MAX;
	return true;
}

float Terrain::GetHeightScale() const noexcept
{
	return p_HeightScale;
}

void Terrain::SetHeightScale(float scale) noexcept
{
	p_HeightScale = scale;
}

void Terrain::SetHeightAt(float height, std::size_t x, std::size_t z)
{
	assert(x < p_HeightData.size && z < p_HeightData.size);
	p_HeightData.data[index(x, z)] = height;
}

float Terrain::GetNormalizedHeightAt(std::size_t x, std::size_t z) const
{
	float rawHeight = GetHeightAt(x, z);
	if (p_IsScaled) {
		if (p_MaxHeight <= p_MinHeight)
			return 0.0f;
		return (rawHeight - p_MinHeight) / (p_MaxHeight - p_MinHeight);
	}

	return rawHeight / p_MaxHeight;
}

float Terrain::GetScaledHeightAt(std::size_t x, std::size_t z) const
{
	return GetNormalizedHeightAt(x, z) * p_HeightScale;
}

float Terrain::GetHeightInterpolated(float x, float z) const
{
	x /= GetWorldScale();
	z /= GetWorldScale();

	float bottomLeftHeight = GetHeightAt((int)x, (int)z);

	if (((int)x + 1 >= GetSize()) || ((int)z + 1 >= GetSize())) {
		return (bottomLeftHeight / p_MaxHeight) * p_HeightScale;
	}

	float bottomRightHeight = GetHeightAt((int)x + 1, (int)z);
	float topLeftHeight = GetHeightAt((int)x, (int)z + 1);
	float topRightHeight = GetHeightAt((int)x + 1, (int)z + 1);

	float xFraction = x - floorf(x);
	float interpolatedBottomEdge = (bottomRightHeight - bottomLeftHeight) * xFraction + bottomLeftHeight;
	float interpolatedTopEdge = (topRightHeight - topLeftHeight) * xFraction + topLeftHeight;

	float zFraction = z - floorf(z);
	float finalHeight = (interpolatedTopEdge - interpolatedBottomEdge) * zFraction + interpolatedBottomEdge;

	return (finalHeight / p_MaxHeight) * p_HeightScale;
}

float Terrain::GetHeightAt(std::size_t x, std::size_t z) const
{
	assert(x < p_HeightData.size && z < p_HeightData.size);
	return p_HeightData.data[index(x, z)];
}

std::size_t Terrain::GetSize() const noexcept
{
	return p_HeightData.size;
}

int Terrain::GetWorldScale() const noexcept
{
	return p_WorldScale;
}


void Terrain::RescaleData(float minRange, float maxRange)
{
	if (p_HeightData.data.empty() || p_HeightData.size == 0) {
		std::cerr << "Error: Empty HeightData" << std::endl;
		return;
	}

	float minVal = *std::min_element(p_HeightData.data.begin(), p_HeightData.data.end());
	float maxVal = *std::max_element(p_HeightData.data.begin(), p_HeightData.data.end());

	if (maxVal <= minVal) {
		std::cerr << "Same maxVal and minVal" << std::endl;
		return;
	}

	float delta = maxVal - minVal;
	float range = maxRange - minRange;

	for (std::size_t i = 0; i < p_HeightData.data.size(); i++) {
		float normalized = (p_HeightData.data[i] - minVal) / delta;
		p_HeightData.data[i] = normalized * range + minRange;
	}

	p_MinHeight = minRange;
	p_MaxHeight = maxRange;
	p_IsScaled = true;
}

std::size_t Terrain::index(std::size_t x, std::size_t z) const noexcept
{
	return z * p_HeightData.size + x;
}