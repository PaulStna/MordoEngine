#include "../Manager.h"
#include "../../FileSystem/FileSystem.h"
#include "../../Texture/Texture.h"
#include <vector>
#include <string>

template<>
void Manager<Texture>::Init() {

    std::vector<std::string> cubeMapFaces = {
        "right.tga",
        "left.tga",
        "top.tga",
        "bottom.tga",
        "back.tga",    
        "front.tga"
    };

	resources.emplace("grass", Texture(FileSystem::getPath("res/textures/grass_low.png")));
	resources.emplace("dirt", Texture(FileSystem::getPath("res/textures/dirt_mid.jpg")));
	resources.emplace("rock", Texture(FileSystem::getPath("res/textures/rock_high.png")));
	resources.emplace("skyBox", Texture(FileSystem::getPath("res/textures/cubemap/"), cubeMapFaces));
    resources.emplace("dudvMap", Texture(FileSystem::getPath("res/textures/water/dudvMap.png")));
}

template class Manager<Texture>;