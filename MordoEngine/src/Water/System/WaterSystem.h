#pragma once
#include "../WaterTile.h"
#include "../../Core/Shader/Shader.h"
#include "../../Renderer/Plane/PlaneRenderer.h"
#include "../../Renderer/Framebuffer/Framebuffer.h"
#include "../../Camera/Camera.h"
#include <functional>
#include <vector>
#include <memory>

class WaterSystem
{
private:
	std::vector<WaterTile> m_WaterTiles;
	std::unique_ptr<PlaneRenderer> m_Renderer;
	std::unique_ptr<Framebuffer> m_ReflectionFramebuffer;

public:
	WaterSystem();
	void Update(float deltaTime);
	void Render(const Shader& terrainShader,
		const Shader& waterShader,
		Camera& camera,
		const glm::mat4* projection,
		const glm::mat4* model,
		std::function<void(float waterY, const glm::mat4& reflectedView)> renderCallback);
	void AddWaterTile(const WaterTile waterTile);
	void AddWaterTile(const glm::vec3 position, const glm::vec3 scale, const float yPos);
	~WaterSystem() = default;
};
