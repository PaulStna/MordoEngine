#include "TextureManager.h"
#include "../FileSystem/FileSystem.h"

std::vector<Texture> TextureManager::textures;
std::unordered_map<std::string, TextureID> TextureManager::pathToId;

void TextureManager::Init() {
    Load("res/textures/bricks.png");
}

TextureID TextureManager::Load(const std::string& path) {
    auto it = pathToId.find(path);
    if (it != pathToId.end())
        return it->second;

    textures.emplace_back(Texture(FileSystem::getPath(path)));
    TextureID id = static_cast<TextureID>(textures.size() - 1);
    pathToId[path] = id;
    return id;
}

Texture& TextureManager::Get(TextureID id) {
    return textures[id];
}
