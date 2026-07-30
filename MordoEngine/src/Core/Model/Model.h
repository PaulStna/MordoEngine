#pragma once
#include "../Texture/Texture.h"
#include "../Transform/Transform.h"
#include "../Resources/ResourceLibrary.h"
#include "Animation/Animator.h"
#include "../../Renderer/Model/ModelRenderer.h"
#include <memory>
#include <string>
#include <vector>

class Shader;

// One model resident on the GPU: its buffers, one drawable piece per submesh,
// where it sits in the world and, if the file had any, the animation playing on
// it.
//
// Everything happens in the constructor, 
// so a Model that exists is a Model youcan draw. 
// Movable but not copyable, since it owns GL objects.
class Model
{
private:
	// One submesh ready to draw, with everything that varies between them kept
	// together instead of in parallel vectors that have to stay aligned.
	struct Piece
	{
		MeshDrawCall drawCall;

		// Non-owning: the textures live in the ResourceLibrary.
		// nullptr = untextured submesh.
		Texture* texture = nullptr;

		// Node whose live transform applies,
		// -1 = the geometry is already in model space.
		int  nodeIndex = -1;

		// Whether bones drive these vertices.
		bool skinned = false;
	};

	// Held by pointer because ModelRenderer owns GL names and is neither copyable nor movable. 
	// That indirection is what keeps Model itself movable
	std::unique_ptr<ModelRenderer> m_Renderer;
	std::vector<Piece>             m_Pieces;

	// Null for a model whose file carried no animation.
	std::unique_ptr<Animator>      m_Animator;

	Transform m_Transform;

public:
	// Throws std::runtime_error if the file cannot be read.
	Model(const std::string& path, ResourceLibrary<Texture>& textures);

	// Advances the animation, if there is one. Cheap no-op otherwise.
	void Update(float deltaTime);

	// Sets only what changes from one model to the next. The caller is expected
	// to have bound the shader and set the per-pass uniforms already.
	void Render(const Shader& shader) const;

	// Position, rotation and scale. 
	// Non-const so callers can just reach in: model.GetTransform().SetScale(2.0f);
	Transform& GetTransform() { return m_Transform; }
	const Transform& GetTransform() const { return m_Transform; }

	bool IsAnimated() const;

	// Returns false if the file has no clip by that name. The first clip plays
	// by default, so this is only needed to pick a different one.
	bool PlayAnimation(const std::string& name, bool loop = true);

	// Null when the model has no animation.
	const Animator* GetAnimator() const { return m_Animator.get(); }
};
