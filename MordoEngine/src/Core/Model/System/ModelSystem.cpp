#include "ModelSystem.h"
#include "../Model.h"

void ModelSystem::BeginPass(const Shader& shader, const RenderContext& renderContext)
{
	shader.Use();
	shader.SetMat4("projection", renderContext.projection);
	shader.SetMat4("view", renderContext.view);
	shader.SetVec4("plane", renderContext.clipPlane);
	shader.SetInt("texture1", 0);
}

void ModelSystem::Render(const Body& body, const Transform& transform,
	const Shader& shader)
{
	if (body.IsEmpty()) return;

	// The model is shared, so everything that makes this body its own arrives
	// here as arguments: where it stands, and the pose it is in.
	body.GetModel()->Render(shader, transform.GetMatrix(), body.GetAnimator());
}
