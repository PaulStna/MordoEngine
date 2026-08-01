#include "Model.h"
#include "ModelLoader.h"
#include "Animation/Animator.h"
#include "../Shader/Shader.h"

Model::Model(const std::string& path, ResourceLibrary<Texture>& textures)
	: m_Renderer(std::make_unique<ModelRenderer>())
{
	const ModelData model = LoadModel(path);
	const std::vector<MeshDrawCall> drawCalls = m_Renderer->Upload(model);

	m_LocalBounds = model.bounds;

	m_Pieces.reserve(drawCalls.size());

	for (size_t i = 0; i < drawCalls.size(); i++)
	{
		const SubMeshData& subMesh = model.subMeshes[i];

		Piece piece;
		piece.drawCall = drawCalls[i];
		piece.nodeIndex = subMesh.nodeIndex;
		piece.skinned = subMesh.skinned;

		if (!subMesh.diffuseTexturePath.empty())
		{
			// Submeshes commonly share a texture, and ResourceLibrary::Load throws
			// on a duplicate id, so reuse whatever is already there. The library is
			// node based, so the reference stays valid as more models are loaded.
			const std::string& id = subMesh.diffuseTexturePath;
			piece.texture = textures.Exists(id)
				? &textures.Get(id)
				: &textures.Load(id, subMesh.diffuseTexturePath);
		}

		m_Pieces.push_back(piece);
	}

	// The rig keeps the skeleton and the clips, so it is only worth building when
	// the file actually had animation in it.
	if (!model.animations.empty())
	{
		m_Rig = std::make_unique<Rig>(model);
	}
}

void Model::Render(const Shader& shader, const glm::mat4& transform,
	const Animator* animator) const
{
	// One upload for the whole skeleton, shared by every piece of this body.
	if (animator)
	{
		shader.SetMat4Array("finalBones", animator->GetBoneMatrices());
	}

	for (const Piece& piece : m_Pieces)
	{
		// A rigid piece whose node is animated has to follow that node; a skinned
		// or already baked one does not, and keeps the model transform as is.
		const glm::mat4 modelMatrix = (piece.nodeIndex >= 0 && animator)
			? transform * animator->GetNodeGlobal(piece.nodeIndex)
			: transform;

		shader.SetMat4("model", modelMatrix);
		shader.SetInt("skinned", piece.skinned ? 1 : 0);

		if (piece.texture)
		{
			glActiveTexture(GL_TEXTURE0);
			piece.texture->Use();
		}

		m_Renderer->Render(piece.drawCall);
	}
}
