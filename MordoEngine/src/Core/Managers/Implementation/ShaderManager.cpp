#include "../Manager.h"
#include "../../FileSystem/FileSystem.h"
#include "../../Shader/Shader.h"

template<>
void Manager<Shader>::Init() {
	resources.emplace("terrain",
		Shader(
			FileSystem::getPath("res/shaders/terrain.vs"),
			FileSystem::getPath("res/shaders/terrain.fs"))
	);

	resources.emplace("terrainSelector",
		Shader(
			FileSystem::getPath("res/shaders/terrainSelector.vs"),
			FileSystem::getPath("res/shaders/terrainSelector.fs"))
	);

	resources.emplace("skyBox",
		Shader(
			FileSystem::getPath("res/shaders/skyBox.vs"),
			FileSystem::getPath("res/shaders/skyBox.fs"))
	);

	resources.emplace("lightCube",
		Shader(
			FileSystem::getPath("res/shaders/lightCube.vs"),
			FileSystem::getPath("res/shaders/lightCube.fs"))
	);
}

template class Manager<Shader>;