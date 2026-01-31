#include "../Manager.h"
#include "../../FileSystem/FileSystem.h"
#include "../../Texture/Texture.h"

template<>
void Manager<Texture>::Init() {
	resources.emplace("atlas", Texture(FileSystem::getPath("res/textures/atlas.png")));
	resources.emplace("colormap", Texture(FileSystem::getPath("res/textures/colormap.png")));
}

template class Manager<Texture>;