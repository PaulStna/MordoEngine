#include "ResourceLoader.h"
#include "../FileSystem/FileSystem.h"
#include <vector>
#include <string>

void LoadShaders(ResourceLibrary<Shader>& shaders)
{
	shaders.Load("terrain",
		FileSystem::getPath("res/shaders/terrain.vs"),
		FileSystem::getPath("res/shaders/terrain.fs"));
	shaders.Load("terrainSelector",
		FileSystem::getPath("res/shaders/terrainSelector.vs"),
		FileSystem::getPath("res/shaders/terrainSelector.fs"));
	shaders.Load("skyBox",
		FileSystem::getPath("res/shaders/skyBox.vs"),
		FileSystem::getPath("res/shaders/skyBox.fs"));
	shaders.Load("lightCube",
		FileSystem::getPath("res/shaders/lightCube.vs"),
		FileSystem::getPath("res/shaders/lightCube.fs"));
	shaders.Load("water",
		FileSystem::getPath("res/shaders/water.vs"),
		FileSystem::getPath("res/shaders/water.fs"));
	shaders.Load("underwater",
		FileSystem::getPath("res/shaders/underwater.vs"),
		FileSystem::getPath("res/shaders/underwater.fs"));
	shaders.Load("model",
		FileSystem::getPath("res/shaders/model.vs"),
		FileSystem::getPath("res/shaders/model.fs"));
}

void LoadTextures(ResourceLibrary<Texture>& textures)
{
	std::vector<std::string> cubeMapFaces = {
		"right.tga", "left.tga", "top.tga", "bottom.tga", "back.tga", "front.tga"
	};

	textures.Load("grass", FileSystem::getPath("res/textures/grass_low.png"));
	textures.Load("dirt", FileSystem::getPath("res/textures/dirt_mid.jpg"));
	textures.Load("rock", FileSystem::getPath("res/textures/rock_high.png"));
	textures.Load("skyBox", FileSystem::getPath("res/textures/cubemap/"), cubeMapFaces);
	textures.Load("dudvMap", FileSystem::getPath("res/textures/water/dudvMap.png"));
}