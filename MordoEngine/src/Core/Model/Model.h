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

/// One model resident on the GPU: its buffers, one drawable piece per submesh,
/// and, when the file carried animation, the rig its clips are authored on.
///
/// This is the asset, not something standing in the world. It is loaded once
/// per file and shared by everyone who wants that shape, so it holds nothing
/// that tells two of them apart: where each one is and which clip it is on live
/// in Body, one per actor.
///
/// Everything happens in the constructor, so a Model that exists is a Model you
/// can draw. Movable but not copyable, since it owns GL objects.
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

	// Model space, measured off the vertices at load. Shared like everything
	// else here: every actor on this model fits the same box, and its transform
	// is what turns that into a place in the world.
	Bounds                         m_LocalBounds = Bounds::Empty();

public:
	/// Loads the file and uploads every submesh to the GPU.
	/// @param path     Model file to load, absolute, as returned by
	///                 FileSystem::getPath. glTF Separate (.gltf + .bin +
	///                 textures), never .glb: embedded textures cannot be read.
	/// @param textures Library the submesh textures are loaded into and then
	///                 borrowed from. Must outlive the Model.
	/// @throws std::runtime_error if the file cannot be read.
	Model(const std::string& path, ResourceLibrary<Texture>& textures);

	/// Draws every piece at the given transform, posed by the given animator.
	///
	/// Both arguments belong to the body being drawn, which is why neither is a
	/// member: the same Model draws every copy of itself, one call each. Sets
	/// only what changes from one body to the next, so the caller is expected to
	/// have bound the shader and set the per-pass uniforms already.
	///
	/// @param transform Model matrix of the body being drawn, world space.
	/// @param animator  Pose to draw in. Null for something rigid, which draws
	///                  the geometry as it was loaded.
	void Render(const Shader& shader, const glm::mat4& transform,
		const Animator* animator) const;

	bool IsAnimated() const { return m_Rig && m_Rig->HasAnimations(); }

	/// Null when the file carried no animation. Body builds its animator from
	/// it, and every body on this model shares the one rig.
	const Rig* GetRig() const { return m_Rig.get(); }

	/// The extent of the geometry, in model space, measured at load. Actor fits
	/// its collider to this, so a model with a body gets one without anyone
	/// asking. For a skinned model this is the bind pose, which animation can
	/// move vertices outside of.
	const Bounds& GetLocalBounds() const { return m_LocalBounds; }
};
