#include "ModelSystem.h"

Model& ModelSystem::Add(const std::string& path,
	ResourceLibrary<Texture>& textures,
	const Transform& transform)
{
	std::unique_ptr<Model> model = std::make_unique<Model>(path, textures);
	model->GetTransform() = transform;
	return *m_Models.emplace_back(std::move(model));
}

void ModelSystem::Update(float deltaTime)
{
	// Only the animated ones actually do anything here.
	for (const std::unique_ptr<Model>& model : m_Models)
	{
		model->Update(deltaTime);
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

	for (const std::unique_ptr<Model>& model : m_Models)
	{
		model->Render(shader);
	}
}
