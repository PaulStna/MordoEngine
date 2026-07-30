#include "ModelLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <unordered_map>

namespace
{
	// Assimp stores matrices row major, glm column major, so this is a transpose
	// and not a memcpy. Getting it wrong is the classic cause of a skeleton that
	// explodes into spikes.
	glm::mat4 ToGlm(const aiMatrix4x4& m)
	{
		return glm::mat4(
			m.a1, m.b1, m.c1, m.d1,
			m.a2, m.b2, m.c2, m.d2,
			m.a3, m.b3, m.c3, m.d3,
			m.a4, m.b4, m.c4, m.d4);
	}

	glm::vec3 ToGlm(const aiVector3D& v) { return { v.x, v.y, v.z }; }
	glm::quat ToGlm(const aiQuaternion& q) { return { q.w, q.x, q.y, q.z }; }

	// Assimp hands back whatever path the exporter wrote, which for glTF is
	// relative to the model file. Resolve it against the model's own folder so
	// the engine's working directory does not matter.
	std::string ResolveTexturePath(const std::string& modelPath, const aiString& raw)
	{
		std::filesystem::path resolved =
			std::filesystem::path(modelPath).parent_path() / raw.C_Str();
		return resolved.lexically_normal().string();
	}

	std::string FindDiffuseTexture(const aiScene* scene,
		const aiMesh* mesh,
		const std::string& modelPath)
	{
		if (mesh->mMaterialIndex >= scene->mNumMaterials) return {};

		const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		aiString raw;

		// glTF exposes the PBR base colour; older formats use the legacy slot.
		if (material->GetTexture(aiTextureType_BASE_COLOR, 0, &raw) == AI_SUCCESS ||
			material->GetTexture(aiTextureType_DIFFUSE, 0, &raw) == AI_SUCCESS)
		{
			return ResolveTexturePath(modelPath, raw);
		}
		return {};
	}

	// Flattens the node tree depth first, which guarantees a parent lands at a
	// lower index than its children. That is what lets the animator resolve every
	// global transform in one forward pass instead of recursing each frame.
	void FlattenNodes(const aiNode* node, int parent,
		ModelData& model, std::vector<const aiNode*>& aiNodes)
	{
		const int index = static_cast<int>(model.nodes.size());

		NodeData data;
		data.name = node->mName.C_Str();
		data.parent = parent;
		data.localTransform = ToGlm(node->mTransformation);

		model.nodes.push_back(std::move(data));
		aiNodes.push_back(node);

		for (unsigned int c = 0; c < node->mNumChildren; ++c)
		{
			FlattenNodes(node->mChildren[c], index, model, aiNodes);
		}
	}

	std::vector<glm::mat4> ComputeGlobalTransforms(const ModelData& model)
	{
		std::vector<glm::mat4> globals(model.nodes.size(), glm::mat4(1.0f));

		for (size_t i = 0; i < model.nodes.size(); i++)
		{
			const NodeData& node = model.nodes[i];
			globals[i] = node.parent < 0
				? node.localTransform
				: globals[node.parent] * node.localTransform;
		}

		return globals;
	}

	int FindNode(const ModelData& model, const std::string& name)
	{
		for (size_t i = 0; i < model.nodes.size(); i++)
		{
			if (model.nodes[i].name == name) return static_cast<int>(i);
		}
		return -1;
	}

	// Writes an influence into the first free slot. Fails only if Assimp could
	// not reduce the mesh down to MAX_BONE_INFLUENCES.
	bool AddBoneInfluence(ModelVertex& vertex, int boneIndex, float weight)
	{
		for (int i = 0; i < MAX_BONE_INFLUENCES; i++)
		{
			if (vertex.boneIds[i] < 0)
			{
				vertex.boneIds[i] = boneIndex;
				vertex.weights[i] = weight;
				return true;
			}
		}
		return false;
	}

	// Bones are identified by name and several meshes can share one, so the model
	// keeps a single table and the map dedupes across submeshes.
	void ExtractBones(const aiMesh* mesh,
		SubMeshData& subMesh,
		ModelData& model,
		std::unordered_map<std::string, int>& boneIndexByName,
		int& droppedWeights)
	{
		for (unsigned int b = 0; b < mesh->mNumBones; b++)
		{
			const aiBone* bone = mesh->mBones[b];
			const std::string name = bone->mName.C_Str();

			auto [entry, inserted] = boneIndexByName.try_emplace(
				name, static_cast<int>(model.bones.size()));

			if (inserted)
			{
				BoneData data;
				data.nodeIndex = FindNode(model, name);
				data.inverseBind = ToGlm(bone->mOffsetMatrix);
				model.bones.push_back(data);
			}

			const int boneIndex = entry->second;

			// The shader indexes a fixed size array, so an out of range bone has
			// to be dropped here rather than read past the end of it.
			if (boneIndex >= MAX_BONES)
			{
				droppedWeights += static_cast<int>(bone->mNumWeights);
				continue;
			}

			for (unsigned int w = 0; w < bone->mNumWeights; w++)
			{
				const aiVertexWeight& weight = bone->mWeights[w];

				if (weight.mVertexId >= subMesh.vertices.size()) continue;
				if (weight.mWeight <= 0.0f) continue;

				if (!AddBoneInfluence(subMesh.vertices[weight.mVertexId],
					boneIndex, weight.mWeight))
				{
					droppedWeights++;
				}
			}
		}
	}

	SubMeshData BuildSubMesh(const aiScene* scene,
		const aiMesh* mesh,
		const std::string& path)
	{
		SubMeshData subMesh;
		subMesh.vertices.reserve(mesh->mNumVertices);
		subMesh.indices.reserve(mesh->mNumFaces * 3);

		for (unsigned int v = 0; v < mesh->mNumVertices; v++)
		{
			ModelVertex vertex;
			vertex.pos = ToGlm(mesh->mVertices[v]);

			if (mesh->HasNormals())
			{
				vertex.normal = ToGlm(mesh->mNormals[v]);
			}

			// A mesh can carry up to 8 UV sets;
			// Channel 0 is the one Blender
			// exports as the active UV map.
			if (mesh->HasTextureCoords(0))
			{
				vertex.texCoord = { mesh->mTextureCoords[0][v].x,
									mesh->mTextureCoords[0][v].y };
			}

			subMesh.vertices.push_back(vertex);
		}

		for (unsigned int f = 0; f < mesh->mNumFaces; f++)
		{
			const aiFace& face = mesh->mFaces[f];
			for (unsigned int i = 0; i < face.mNumIndices; i++)
			{
				subMesh.indices.push_back(face.mIndices[i]);
			}
		}

		subMesh.diffuseTexturePath = FindDiffuseTexture(scene, mesh, path);
		return subMesh;
	}

	void ExtractAnimations(const aiScene* scene, ModelData& model)
	{
		model.animations.reserve(scene->mNumAnimations);

		for (unsigned int a = 0; a < scene->mNumAnimations; a++)
		{
			const aiAnimation* source = scene->mAnimations[a];

			AnimationData animation;
			animation.name = source->mName.C_Str();
			animation.durationTicks = static_cast<float>(source->mDuration);

			// Files that leave this at zero are quoting times in the default
			// 25 fps Assimp assumes.
			animation.ticksPerSecond = source->mTicksPerSecond != 0.0
				? static_cast<float>(source->mTicksPerSecond)
				: 25.0f;

			animation.channels.reserve(source->mNumChannels);

			for (unsigned int c = 0; c < source->mNumChannels; c++)
			{
				const aiNodeAnim* source_channel = source->mChannels[c];

				const int nodeIndex = FindNode(model, source_channel->mNodeName.C_Str());
				if (nodeIndex < 0) continue;   // channel for a node this file dropped

				NodeChannel channel;
				channel.nodeIndex = nodeIndex;

				channel.positions.reserve(source_channel->mNumPositionKeys);
				for (unsigned int k = 0; k < source_channel->mNumPositionKeys; k++)
				{
					const aiVectorKey& key = source_channel->mPositionKeys[k];
					channel.positions.push_back(
						{ static_cast<float>(key.mTime), ToGlm(key.mValue) });
				}

				channel.rotations.reserve(source_channel->mNumRotationKeys);
				for (unsigned int k = 0; k < source_channel->mNumRotationKeys; k++)
				{
					const aiQuatKey& key = source_channel->mRotationKeys[k];
					channel.rotations.push_back(
						{ static_cast<float>(key.mTime), ToGlm(key.mValue) });
				}

				channel.scales.reserve(source_channel->mNumScalingKeys);
				for (unsigned int k = 0; k < source_channel->mNumScalingKeys; k++)
				{
					const aiVectorKey& key = source_channel->mScalingKeys[k];
					channel.scales.push_back(
						{ static_cast<float>(key.mTime), ToGlm(key.mValue) });
				}

				animation.channels.push_back(std::move(channel));
			}

			model.animations.push_back(std::move(animation));
		}
	}
}

ModelData LoadModel(const std::string& path)
{
	Assimp::Importer importer;

	// Read with no post-processing first, only to find out whether the file is
	// animated. The two flag sets below are mutually exclusive in intent, so the
	// decision has to happen before any of them runs.
	const aiScene* scene = importer.ReadFile(path, 0);

	if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode)
	{
		throw std::runtime_error("Assimp failed to load '" + path + "': "
			+ importer.GetErrorString());
	}

	const bool animated = scene->mNumAnimations > 0;

	// No aiProcess_FlipUVs: Texture already loads images flipped via stb!.
	unsigned int flags =
		aiProcess_Triangulate |
		aiProcess_GenSmoothNormals |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ImproveCacheLocality |
		aiProcess_ValidateDataStructure;

	if (animated)
	{
		// LimitBoneWeights trims each vertex down to the four influences the
		// vertex layout can carry, instead of letting the extras be dropped
		// silently later.
		//
		// Deliberately no PreTransformVertices and no OptimizeMeshes: the first
		// flattens the node hierarchy the animation drives, and the second merges
		// meshes across nodes, which breaks the mesh to node association.
		flags |= aiProcess_LimitBoneWeights;
	}
	else
	{
		// Static geometry has nothing to gain from the hierarchy, so bake it and
		// let Assimp merge submeshes that share a material into one draw call.
		flags |= aiProcess_PreTransformVertices | aiProcess_OptimizeMeshes;
	}

	scene = importer.ApplyPostProcessing(flags);

	if (!scene || !scene->mRootNode)
	{
		throw std::runtime_error("Assimp post-processing failed for '" + path + "': "
			+ importer.GetErrorString());
	}

	ModelData model;

	std::vector<const aiNode*> aiNodes;
	FlattenNodes(scene->mRootNode, -1, model, aiNodes);

	const std::vector<glm::mat4> globals = ComputeGlobalTransforms(model);

	if (animated)
	{
		ExtractAnimations(scene, model);
	}

	std::unordered_map<std::string, int> boneIndexByName;
	int droppedWeights = 0;

	model.subMeshes.reserve(scene->mNumMeshes);

	// Walk the nodes rather than scene->mMeshes, because which node carries a
	// mesh is what decides how it gets transformed. A mesh referenced by two
	// nodes legitimately becomes two submeshes.
	for (size_t n = 0; n < aiNodes.size(); n++)
	{
		const aiNode* node = aiNodes[n];

		for (unsigned int m = 0; m < node->mNumMeshes; m++)
		{
			const aiMesh* mesh = scene->mMeshes[node->mMeshes[m]];

			SubMeshData subMesh = BuildSubMesh(scene, mesh, path);
			const bool hasBones = mesh->mNumBones > 0;

			if (hasBones)
			{
				// Skinned vertices are authored in the skin's own space and the
				// bone matrices carry them the rest of the way, so the node
				// transform must not be applied on top.
				ExtractBones(mesh, subMesh, model, boneIndexByName, droppedWeights);
				subMesh.skinned = animated;
			}
			else if (animated)
			{
				// Rigid mesh in an animated file: its node may be moving, so keep
				// the index and resolve the transform every frame.
				subMesh.nodeIndex = static_cast<int>(n);
			}
			else
			{
				// Nothing here will ever move. Bake the node transform in now so
				// rendering stays a plain draw call.
				//
				// With PreTransformVertices this is already the identity; doing it
				// unconditionally keeps the untangling out of the render path.
				const glm::mat4& toModelSpace = globals[n];
				const glm::mat3 normalMatrix = glm::mat3(toModelSpace);

				for (ModelVertex& vertex : subMesh.vertices)
				{
					vertex.pos = glm::vec3(toModelSpace * glm::vec4(vertex.pos, 1.0f));
					vertex.normal = glm::normalize(normalMatrix * vertex.normal);
				}
			}

			model.subMeshes.push_back(std::move(subMesh));
		}
	}

	if (droppedWeights > 0)
	{
		std::cout << "Model '" << path << "': dropped " << droppedWeights
			<< " bone weights (over " << MAX_BONE_INFLUENCES
			<< " per vertex or over " << MAX_BONES << " bones).\n";
	}

	return model;   // importer frees the aiScene here;
}
