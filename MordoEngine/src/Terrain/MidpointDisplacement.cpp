#include "MidpointDisplacement.h"
#include <random>
#include <cmath>
#include <cstdlib>
#include <iostream>

// Helpers 
static int CalcNextPowerOfTwo(int value)
{
	int result = 1;
	while (result < value) {
		result <<= 1;
	}
	return result;
}

static std::mt19937 globalRng(std::random_device{}());
static std::uniform_real_distribution<float> globalUnitDist(0.0f, 1.0f);

static float RandomFloat()
{
	return globalUnitDist(globalRng); // [0,1)
}

static float RandomFloatRange(float minValue, float maxValue)
{
	if (minValue == maxValue) {
		return minValue;
	}
	if (maxValue < minValue) {
		std::cerr << "invalid random range: ("
			<< minValue << ", " << maxValue << ")\n";
		std::exit(EXIT_FAILURE);
	}
	return minValue + (maxValue - minValue) * RandomFloat();
}

MidpointDisplacement::MidpointDisplacement(
	std::size_t terrainSize, float roughness, std::size_t minHeight = 0,
	std::size_t maxHeight = terrain::RAW_HEIGHT_MAX)
{
	if (roughness < 0.0f) {
		exit(-99);
	}
	Initialize(terrainSize, minHeight, maxHeight);
	m_Roughness = roughness;
	CreateMidpointDisplacement();
	RescaleData(p_MinHeight, p_MaxHeight);
}

void MidpointDisplacement::CreateMidpointDisplacement()
{
	int rectangleSize = CalcNextPowerOfTwo(GetSize());
	float currentHeight = (float)rectangleSize / 2.0f;
	float heightReduction = pow(2.0f, -m_Roughness);

	while (rectangleSize > 0) {
		DiamondStep(rectangleSize, currentHeight);
		SquareStep(rectangleSize, currentHeight);
		rectangleSize /= 2;
		currentHeight *= heightReduction;
	}
}

void MidpointDisplacement::DiamondStep(int rectangleSize, float currentHeight)
{
	int halfRectangleSize = rectangleSize / 2;

	for (int y = 0; y < GetSize(); y += rectangleSize) {
		for (int x = 0; x < GetSize(); x += rectangleSize) {
			int nextX = (x + rectangleSize) % GetSize();
			int nextY = (y + rectangleSize) % GetSize();

			if (nextX < x) {
				nextX = GetSize() - 1;
			}
			if (nextY < y) {
				nextY = GetSize() - 1;
			}

			float topLeft = GetHeightAt(x, y);
			float topRight = GetHeightAt(nextX, y);
			float bottomLeft = GetHeightAt(x, nextY);
			float bottomRight = GetHeightAt(nextX, nextY);

			int midX = (x + halfRectangleSize) % GetSize();
			int midY = (y + halfRectangleSize) % GetSize();

			float randomValue = RandomFloatRange(-currentHeight, currentHeight);
			float midPoint = (topLeft + topRight + bottomLeft + bottomRight) / 4.0f;

			SetHeightAt(midPoint + randomValue, midX, midY);
		}
	}
}

void MidpointDisplacement::SquareStep(int rectangleSize, float currentHeight)
{
	int halfRectangleSize = rectangleSize / 2;

	for (int y = 0; y < GetSize(); y += rectangleSize) {
		for (int x = 0; x < GetSize(); x += rectangleSize) {
			int nextX = (x + rectangleSize) % GetSize();
			int nextY = (y + rectangleSize) % GetSize();

			if (nextX < x) {
				nextX = GetSize() - 1;
			}
			if (nextY < y) {
				nextY = GetSize() - 1;
			}

			int midX = (x + halfRectangleSize) % GetSize();
			int midY = (y + halfRectangleSize) % GetSize();
			int prevMidX = (x - halfRectangleSize + GetSize()) % GetSize();
			int prevMidY = (y - halfRectangleSize + GetSize()) % GetSize();

			float currentTopLeft = GetHeightAt(x, y);
			float currentTopRight = GetHeightAt(nextX, y);
			float currentCenter = GetHeightAt(midX, midY);
			float previousYCenter = GetHeightAt(midX, prevMidY);
			float currentBottomLeft = GetHeightAt(x, nextY);
			float previousXCenter = GetHeightAt(prevMidX, midY);

			float currentLeftMid = (currentTopLeft + currentCenter + currentBottomLeft + previousXCenter) / 4.0f
				+ RandomFloatRange(-currentHeight, currentHeight);
			float currentTopMid = (currentTopLeft + currentCenter + currentTopRight + previousYCenter) / 4.0f
				+ RandomFloatRange(-currentHeight, currentHeight);

			SetHeightAt(currentTopMid, midX, y);
			SetHeightAt(currentLeftMid, x, midY);
		}
	}
}