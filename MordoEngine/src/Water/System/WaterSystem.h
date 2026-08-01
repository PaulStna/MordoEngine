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

/// Owns the water surfaces, the framebuffers their reflection and refraction
/// are captured into, and the underwater post-process.
///
/// The reflection is the scene drawn again with the camera mirrored across the
/// water plane; the refraction is the scene drawn again clipped to below it.
/// Both need the whole opaque world, which this system does not know how to
/// draw — so the caller hands in a callback and this supplies the context each
/// pass needs. That inversion is what keeps World out of the water code and the
/// water code out of World.
class WaterSystem
{
public:
	/// Draws the opaque scene with the context the water system supplies: a
	/// clip plane, and for the reflection a mirrored view.
	using RenderPass = std::function<void(const RenderContext&)>;

private:
	std::vector<WaterTile> m_WaterTiles;
	std::unique_ptr<PlaneRenderer> m_Renderer;

	// One target per pass. Held by pointer because a Framebuffer owns GL names
	// and cannot be copied or moved.
	std::unique_ptr<Framebuffer> m_UnderWaterFramebuffer;
	std::unique_ptr<Framebuffer> m_ReflectionFramebuffer;
	std::unique_ptr<Framebuffer> m_RefractionFramebuffer;

	// Margin between the surface and the clip plane, in world units. Without it
	// the two coincide and the waterline shows a seam of half-clipped geometry.
	float m_ClipPlaneOffset = 10.0f;

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

	/// Advances the ripples and works out whether the camera is submerged.
	/// @param cameraPosition World space; compared against the water level.
	void Update(float deltaTime, const glm::vec3& cameraPosition);

	/// Captures the reflection and refraction textures into the framebuffers.
	///
	/// Must run before the main opaque scene is drawn, since that pass samples
	/// what this produces. Manages GL_CLIP_DISTANCE0 around both passes.
	///
	/// @param renderPass Called once per pass with a derived context. This is
	///                   where the scene gets drawn twice more, so anything
	///                   inside it runs three times a frame in total.
	void CaptureReflectionRefraction(const RenderContext& renderContext,
		const Camera& camera,
		const RenderPass& renderPass);

	/// Draws the water surfaces.
	///
	/// Must run after the opaque scene so it can blend over the terrain, and so
	/// that seen from below it reads as a translucent sheet.
	void RenderSurface(const Shader& waterShader,
		const Texture& waterDuDvMapTexture,
		const RenderContext& renderContext);

	void AddWaterTile(const WaterTile waterTile);

	/// @param position Centre of the tile, world space.
	/// @param scale    Size in world units.
	/// @param yPos     World-space height of the surface.
	void AddWaterTile(const glm::vec3 position, const glm::vec3 scale, const float yPos);

	/// Draws the whole screen through the underwater distortion.
	///
	/// Only worth calling while GetIsUnderwater is true; above the surface the
	/// scene goes straight to the screen instead.
	///
	/// @param textureId    Colour attachment the scene was rendered into.
	/// @param deltaTime    Running time, used to scroll the distortion.
	/// @param screenWidth  Viewport to restore for the fullscreen pass.
	/// @param screenHeight Same.
	void RenderUnderwater(
		const Shader& underwaterShader,
		const Texture& waterDuDvMapTexture,
		const RenderContext& renderContext,
		const GLuint textureId,
		const float deltaTime,
		const unsigned int screenWidth,
		const unsigned int screenHeight);

	/// @param waterLevel World-space Y used to decide whether the camera is
	///                   submerged. Set this to match the tiles that were added.
	void SetWaterLevel(const float waterLevel) { m_WaterLevel = waterLevel; }

	/// Whether the camera was below the water level as of the last Update.
	bool GetIsUnderwater() const { return m_IsUnderwater; }

	~WaterSystem() = default;
};
