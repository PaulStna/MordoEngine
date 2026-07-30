#include "ModelSystem.h"

Model& ModelSystem::Add(const std::string& path,
	ResourceLibrary<Texture>& textures,
	const Transform& transform)
{
	Model& model = m_Models.emplace_back(path, textures);
	model.GetTransform() = transform;
	return model;
}

void ModelSystem::Update(float deltaTime)
{
	// Only the animated ones actually do anything here.
	for (Model& model : m_Models)
	{
		model.Update(deltaTime);
	}
}

void ModelSystem::Render(const Shader& shader, const RenderContext& renderContext)
{
	if (m_Models.empty())
		return;

	// The uniforms shared by every model, set once for the whole pass.
	shader.Use();
	shader.SetMat4("projection", renderContext.projection);
	shader.SetMat4("view", renderContext.view);
	shader.SetVec4("plane", renderContext.clipPlane);
	shader.SetInt("texture1", 0);

	for (const Model& model : m_Models)
	{
		model.Render(shader);
	}
}
