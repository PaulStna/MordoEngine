#pragma once
#include "./PointLight/PointLight.h"
#include "./DirLight/DirLight.h"
#include "../Core/Shader/Shader.h"
#include "../Renderer/RenderContext.h"
#include <vector>
#include <memory>

/// Owns every light in the scene and is the only thing that talks to the
/// shaders about them.
///
/// One directional light standing in for the sun, plus any number of point
/// lights. Anything that wants to be lit asks for the uniforms rather than
/// knowing what a light is, which is what lets the terrain, the models and the
/// water share one lighting model.
class LightSystem
{
private:
	// How many point lights the shaders declare room for. Must match the array
	// size in every shader that reads them; raising one without the other
	// overruns the uniform array.
	static constexpr int MAX_POINT_LIGHTS = 16;

	std::vector<PointLight> m_PointLights;
	std::unique_ptr<DirLight> m_DirLight;

	void RenderLights(const Shader& lightCubeShader, const RenderContext& renderContext);

public:
	LightSystem();

	/// Advances the time of day, which moves the directional light.
	void Update(float deltaTime);

	/// Applies the light uniforms to the terrain shader and draws the marker
	/// cubes of any lights that have one.
	void Render(const Shader& terrainShader,
				const Shader& cubeLightShader,
				const RenderContext& renderContext);

	/// Feeds the light uniforms into any shader that declares the same DirLight
	/// and PointLight structs.
	///
	/// Only enabled lights are uploaded, packed into consecutive slots, and the
	/// count sent to the shader is how many actually made it. A light that is
	/// off therefore costs nothing rather than occupying a slot.
	void ApplyUniforms(const Shader& shader, const RenderContext& renderContext);

	/// Takes ownership of a light and registers it.
	/// @return The light's index, which an actor keeps so it can move or switch
	///         it later. Indices are stable because lights are only ever
	///         appended.
	std::size_t AddPointLight(PointLight&& pointLight);

	/// How many lights exist, which may be more than reach the shaders.
	///
	/// Only the first MAX_POINT_LIGHTS enabled ones are uploaded. Adding more is
	/// not an error; they simply do not light anything until this picks the
	/// nearest ones instead of the first ones.
	std::size_t GetPointLightCount() const { return m_PointLights.size(); }

	/// @param index As returned by AddPointLight. Out of range is ignored.
	void SetPointLightPosition(std::size_t index, const glm::vec3& position);

	/// Turning one off keeps everything about it and only stops it reaching the
	/// shaders, so a lamp can be switched without its actor rebuilding anything.
	void SetPointLightEnabled(std::size_t index, bool enabled);
	bool IsPointLightEnabled(std::size_t index) const;

	~LightSystem();
};
