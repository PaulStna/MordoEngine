#pragma once
#include "ResourceLibrary.h"
#include "../Shader/Shader.h"
#include "../Texture/Texture.h"
#include "../Model/Model.h"

// Startup population of the resource libraries. Everything the engine draws
// with is loaded once through here, under the string ids the rest of the code
// asks for, so no subsystem ever loads a file for itself.
//
// A new shader, texture or model becomes available to the whole engine by
// gaining a line in the matching function below.

/// Compiles and registers every shader program. Requires a current GL context.
/// @throws std::runtime_error on a missing or uncompilable source file. There
///         is no scene without shaders, so this failure is fatal by design.
void LoadShaders(ResourceLibrary<Shader>& shaders);

/// Loads and registers every texture, including the skybox cubemap.
/// @throws std::runtime_error only on a duplicate id. A missing image file is
///         reported by Texture and survived as a blank texture.
void LoadTextures(ResourceLibrary<Texture>& textures);

/// Loads and registers every model.
///
/// Models are optional: a file that is not on disk is reported and skipped, so
/// the scene still comes up without it and whoever asked for it simply finds
/// nothing under that id. Callers therefore guard with ResourceLibrary::Exists
/// rather than assuming a model is present.
///
/// @param textures Library the models load their own textures into and then
///                 borrow from. Must outlive every model.
void LoadModels(ResourceLibrary<Model>& models, ResourceLibrary<Texture>& textures);
