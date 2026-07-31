#pragma once
#include "../Texture/Texture.h"
#include "../Resources/ResourceLibrary.h"
#include "Animation/Rig.h"
#include "../../Renderer/Model/ModelRenderer.h"
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

class Shader;
class Animator;

// One model resident on the GPU: its buffers, one drawable piece per submesh,
// and, when the file carried animation, the rig its clips are authored on.
//
// This is the asset, not something standing in the world. It is loaded once per
// file and shared by everyone who wants that shape, so it holds nothing that
// tells two of them apart: where each one is and which clip it is on live in
// Body, one per actor.
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
	std::unique_ptr<Rig>           m_Rig;

public:
	// Throws std::runtime_error if the file cannot be read.
	Model(const std::string& path, ResourceLibrary<Texture>& textures);

	// Draws every piece at the given transform, posed by the given animator.
	// Both belong to the body being drawn, which is why neither is a member:
	// the same Model draws every fox, one call each.
	//
	// Pass a null animator for something rigid. Sets only what changes from one
	// body to the next; the caller is expected to have bound the shader and set
	// the per-pass uniforms already.
	void Render(const Shader& shader, const glm::mat4& transform,
		const Animator* animator) const;

	bool IsAnimated() const { return m_Rig && m_Rig->HasAnimations(); }

	// Null when the file carried no animation. Body builds its animator from it.
	const Rig* GetRig() const { return m_Rig.get(); }
};
