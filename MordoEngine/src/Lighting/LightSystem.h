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

	void AddPointLight(PointLight&& pointLight);
	~LightSystem();
};
