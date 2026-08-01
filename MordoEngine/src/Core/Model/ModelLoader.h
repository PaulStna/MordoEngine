#pragma once
#include "ModelData.h"
#include <string>

/// Reads a model file through Assimp and returns plain CPU geometry.
///
/// Makes no GL calls and needs no context, which is what allows a file to be
/// checked — bone counts, weight sums, clip durations — without opening a
/// window. The post-processing flags are chosen per file: a model with no clips
/// gets its node hierarchy baked into the vertices and its submeshes merged,
/// while an animated one keeps the hierarchy its clips drive.
///
/// @param path Model file, absolute, as returned by FileSystem::getPath.
/// @return Geometry, nodes, bones, clips and the model-space bounds, all
///         measured during the single load pass.
/// @throws std::runtime_error if the file cannot be read or Assimp rejects it.
ModelData LoadModel(const std::string& path);
