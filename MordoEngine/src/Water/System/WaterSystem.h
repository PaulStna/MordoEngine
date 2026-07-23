#pragma once
#include "../WaterTile.h"
#include "../../Core/Shader/Shader.h"
#include "../../Core/Texture/Texture.h"
#include "../../Renderer/Plane/PlaneRenderer.h"
#include "../../Renderer/Framebuffer/Framebuffer.h"
#include "../../Renderer/RenderContext.h"
#include "../../Camera/Camera.h"
#include <functional>
#include <vector>
#include <memory>

class WaterSystem
{
public:
	// Draws the opaque scene with the context the water system supplies
	// (clip plane and, for the reflection, a mirrored view).
	using RenderPass = std::function<void(const RenderContext&)>;

private:
	std::vector<WaterTile> m_WaterTiles;
	std::unique_ptr<PlaneRenderer> m_Renderer;
	std::unique_ptr<Framebuffer> m_UnderWaterFramebuffer;
	std::unique_ptr<Framebuffer> m_ReflectionFramebuffer;
	std::unique_ptr<Framebuffer> m_RefractionFramebuffer;
	float offSet = 10.f;
	float m_WaterLevel = 0.0f;
	bool m_IsUnderwater = false;

	void RenderReflection(const WaterTileData& waterTile,
		const RenderContext& renderContext,
		const Camera& camera,
		const RenderPass& renderPass);
	void RenderRefraction(const WaterTileData& waterTile,
		const RenderContext& renderContext,
		const RenderPass& renderPass);

public:
	WaterSystem();
	void Update(float deltaTime, const glm::vec3& cameraPosistio);
	// Captures the reflection/refraction textures into the framebuffers.
	// Must run before the main opaque scene is drawn.
	void CaptureReflectionRefraction(const RenderContext& renderContext,
		const Camera& camera,
		const RenderPass& renderPass);
	// Draws the water surface plane(s). Must run after the opaque scene so it can
	// blend over the terrain when the camera is submerged.
	void RenderSurface(const Shader& waterShader,
		const Texture& waterDuDvMapTexture,
		const RenderContext& renderContext);
	void AddWaterTile(const WaterTile waterTile);
	void AddWaterTile(const glm::vec3 position, const glm::vec3 scale, const float yPos);
	void RenderUnderwater(
		const Shader& underwaterShader,
		const Texture& waterDuDvMapTexture,
		const RenderContext& renderContext,
		const GLuint textureId,
		const float deltaTime,
		const unsigned int screenWidth, 
		const unsigned int screenHeight);
	void const SetWaterLevel (const float waterLevel) { m_WaterLevel = waterLevel; }
	bool const GetIsUnderwater() { return m_IsUnderwater; }
	~WaterSystem() = default;
};
