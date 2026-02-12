#include "../Manager.h"
#include "../../FileSystem/FileSystem.h"
#include "../../Shader/Shader.h"

template<>
void Manager<Shader>::Init() {
	resources.emplace("terrain", 
		std::move(Shader(
			FileSystem::getPath("res/shaders/terrain.vs"), 
			FileSystem::getPath("res/shaders/terrain.fs"))
		));

	resources.emplace("terrainSelector",
		std::move(Shader(
			FileSystem::getPath("res/shaders/terrainSelector.vs"),
			FileSystem::getPath("res/shaders/terrainSelector.fs"))
		));
}

template class Manager<Shader>;