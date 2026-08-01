#pragma once
#include "ModelVertex.h"
#include "../Math/Bounds.h"
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

// One submesh: a contiguous run of geometry that shares a single material.
struct SubMeshData
{
	std::vector<ModelVertex>  vertices;
	std::vector<unsigned int> indices;
	std::string               diffuseTexturePath;   // empty = untextured

	// The node that carries this mesh, or -1 when the geometry already sits in
	// model space. Static models get -1 because the loader bakes the hierarchy
	// into the vertices; an animated model keeps the index so the node's live
	// transform can be applied every frame.
	int  nodeIndex = -1;

	// True when bones drive the vertices, so the shader has to skin them.
	bool skinned = false;
};

// One joint of a skin: the node that drives it, and the matrix that takes a
// vertex from model space into that joint's local space.
struct BoneData
{
	int       nodeIndex = -1;
	glm::mat4 inverseBind{ 1.0f };
};

// One entry of the flattened node hierarchy. Parents always come before their
// children, so a single forward pass computes every global transform.
struct NodeData
{
	std::string name;
	int         parent = -1;
	glm::mat4   localTransform{ 1.0f };
};

template<typename T>
struct KeyFrame
{
	float time = 0.0f;
	T     value{};
};

// The keyframes that animate one node. Any of the three lists may be empty, in
// which case the node keeps its default local value for that component.
struct NodeChannel
{
	int nodeIndex = -1;
	std::vector<KeyFrame<glm::vec3>> positions;
	std::vector<KeyFrame<glm::quat>> rotations;
	std::vector<KeyFrame<glm::vec3>> scales;
};

// Times are in ticks, the unit the file was authored in; divide by
// ticksPerSecond to get seconds.
struct AnimationData
{
	std::string              name;
	float                    durationTicks = 0.0f;
	float                    ticksPerSecond = 25.0f;
	std::vector<NodeChannel> channels;
};

// Whole model as plain CPU data.
struct ModelData
{
	std::vector<SubMeshData>   subMeshes;
	std::vector<NodeData>      nodes;
	std::vector<BoneData>      bones;
	std::vector<AnimationData> animations;

	// Everything the model covers, in model space, which is the only place the
	// vertices are still around to measure. Computed during the load rather than
	// afterwards because it is one pass over data already in cache.
	//
	// For a skinned mesh this is the bind pose. Animation moves vertices outside
	// it, so anything that needs to be safe rather than tight should Expand it.
	Bounds bounds = Bounds::Empty();
};
