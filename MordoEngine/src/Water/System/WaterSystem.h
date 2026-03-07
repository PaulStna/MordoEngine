#pragma once
#include "../WaterTile.h"
#include "../../Core/Shader/Shader.h"
#include "../../Renderer/Plane/PlaneRenderer.h"
#include "../../Renderer/Framebuffer/Framebuffer.h"
#include <vector>
#include <memory>

class WaterSystem
{
private:
	std::vector<WaterTile> m_WaterTiles;
	std::unique_ptr<PlaneRenderer> m_Renderer;
	std::unique_ptr<Framebuffer> m_Framebuffer;

public:
	WaterSystem();
	void Update(float deltaTime);
	void Render(const Shader& shader,
				const glm::vec3& cameraPos,
				const glm::mat4* projection,
				const glm::mat4* view,
				const glm::mat4* model);
	void AddWaterTile(const WaterTile waterTile);
	void AddWaterTile(const glm::vec3 position, const glm::vec3 scale, const float yPos);
	~WaterSystem() = default;
};
