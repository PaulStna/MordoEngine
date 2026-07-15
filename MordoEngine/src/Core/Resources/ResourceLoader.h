#pragma once
#include "ResourceLibrary.h"
#include "../Shader/Shader.h"
#include "../Texture/Texture.h"

void LoadShaders(ResourceLibrary<Shader>& shaders);
void LoadTextures(ResourceLibrary<Texture>& textures);