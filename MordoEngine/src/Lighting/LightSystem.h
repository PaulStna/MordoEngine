#pragma once
#include "./PointLight/PointLight.h"
#include "./DirLight/DirLight.h"
#include "../Core/Shader/Shader.h"
#include "../Renderer/RenderContext.h"
#include <vector>
#include <memory>

class LightSystem
{
private:
	const int MAX_POINT_IGHTS = 16;
	std::vector<PointLight> m_PointLights;
	std::unique_ptr<DirLight> m_DirLight;

	void RenderLights(const Shader& lightCubeShader, const RenderContext& renderContext);

public:
	LightSystem();
	void Update(float deltaTime);
	void Render(const Shader& terrainShader,
				const Shader& cubeLightShader,
				const RenderContext& renderContext);

	// Feeds the light uniforms into any shader that declares the same
	// DirLight/PointLight structs.
	void ApplyUniforms(const Shader& shader, const RenderContext& renderContext);

	// Returns the light's index, which an actor keeps so it can move it later.
	// Indices are stable because lights are only ever appended.
	std::size_t AddPointLight(PointLight&& pointLight);

	// Only the first MAX_POINT_IGHTS reach the shaders. Adding more is not an
	// error, they just do not light anything until this picks the nearest ones
	// instead of the first ones.
	std::size_t GetPointLightCount() const { return m_PointLights.size(); }
	void SetPointLightPosition(std::size_t index, const glm::vec3& position);

	~LightSystem();
};
