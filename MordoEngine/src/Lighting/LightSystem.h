#pragma once
#include "./PointLight/PointLight.h"
#include "./DirLight/DirLight.h"
#include "../Core/Shader/Shader.h"
#include <vector>
#include <memory>

class LightSystem
{
private:
	const int MAX_POINT_IGHTS = 16;
	std::vector<PointLight> m_PointLights;
	std::unique_ptr<DirLight> m_DirLight;

	void RenderLights(const Shader& lightCubeShader,
					  const glm::mat4* projection,
					  const glm::mat4* view);

	void ApplyUniforms(const Shader& shader, const glm::vec3& cameraPos);
public:
	LightSystem();
	void Update(float deltaTime);
	void Render(const Shader& terrainShader,
				const Shader& cubeLightShader,		
				const glm::vec3& cameraPos, 
				const glm::mat4* projection,
				const glm::mat4* view,
				const glm::mat4* model);
	void AddPointLight(PointLight&& pointLight);
	~LightSystem();
};
