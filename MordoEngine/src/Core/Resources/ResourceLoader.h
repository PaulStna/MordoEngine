#pragma once
#include "ResourceLibrary.h"
#include "../Shader/Shader.h"
#include "../Texture/Texture.h"
#include "../Model/Model.h"

void LoadShaders(ResourceLibrary<Shader>& shaders);
void LoadTextures(ResourceLibrary<Texture>& textures);
void LoadModels(ResourceLibrary<Model>& models, ResourceLibrary<Texture>& textures);