#pragma once
#include "../Model.h"
#include "../../Shader/Shader.h"
#include "../../Texture/Texture.h"
#include "../../Resources/ResourceLibrary.h"
#include "../../../Renderer/RenderContext.h"
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

// Owns every model in the scene and draws them all in one pass, sharing the
// shader and its per-pass uniforms.
// (Same role TerrainSystem plays for the terrain:
// the models themselves know nothing about the pass they are in.)
class ModelSystem
{
private:
	// Held by pointer so the models keep their address when the vector grows.
	// Actors point at them, and a reference that moves under an actor is a
	// dangling pointer waiting to happen.
	std::vector<std::unique_ptr<Model>> m_Models;

public:
	// Loads a model and places it. Throws std::runtime_error if the file cannot
	// be read. The returned reference stays valid for as long as the system
	// lives, so it is safe to hold on to:
	// IMPORTANT (Usage) -> m_Models.Add(path, textures).GetTransform().SetScale(2.0f);
	Model& Add(const std::string& path,
			   ResourceLibrary<Texture>& textures,
			   const Transform& transform = Transform{});

	void Update(float deltaTime);
	void Render(const Shader& shader, const RenderContext& renderContext);

	bool IsEmpty() const { return m_Models.empty(); }
	std::size_t Count() const { return m_Models.size(); }
};
