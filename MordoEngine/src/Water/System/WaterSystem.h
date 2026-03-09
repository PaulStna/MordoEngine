#pragma once
#include "../WaterTile.h"
#include "../../Core/Shader/Shader.h"
#include "../../Core/Texture/Texture.h"
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
	std::unique_ptr<Framebuffer> m_RefractionFramebuffer;
	float offSet = 10.f;

public:
	WaterSystem();
	void Update(float deltaTime);
	void Render(
		const Shader& waterShader,
		const Texture& waterDuDvMapTexture,
		Camera& camera,
		const glm::mat4* projection,
		const glm::mat4* model,
		std::function<void(float waterY, const glm::mat4* reflectedView)> renderCallback);
	void RenderReflection(const WaterTileData& waterTile, Camera& camera, std::function<void(float waterY, const glm::mat4* reflectedView)> renderCallback);
	void RenderRefraction(const WaterTileData& waterTile, std::function<void(float waterY, const glm::mat4* reflectedView)> renderCallback);
	void AddWaterTile(const WaterTile waterTile);
	void AddWaterTile(const glm::vec3 position, const glm::vec3 scale, const float yPos);
	~WaterSystem() = default;
};
