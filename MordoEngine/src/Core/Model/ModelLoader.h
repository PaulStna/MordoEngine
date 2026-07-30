#pragma once
#include "ModelData.h"
#include <string>

// Reads a model file through Assimp and returns plain CPU geometry.
// Throws std::runtime_error if the file cannot be read.
ModelData LoadModel(const std::string& path);