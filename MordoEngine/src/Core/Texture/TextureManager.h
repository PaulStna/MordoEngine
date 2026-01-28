#pragma once
#include "Texture.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <cstdint>

using TextureID = uint32_t;

class TextureManager {
public:
    static void Init();

    static TextureID Load(const std::string& path);
    static Texture& Get(TextureID id);

private:
    static std::vector<Texture> textures;
    static void LoadTexture(const std::string& path);
    static std::unordered_map<std::string, TextureID> pathToId;
};
