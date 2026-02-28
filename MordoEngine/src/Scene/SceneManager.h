#pragma once
#include "Scene.h"
#include "../Renderer/Renderer.h"
#include "../Terrain/Terrain.h"
#include "../Camera/Camera.h"
#include "../Terrain/System/TerrainSystem.h"
#include <unordered_map>
#include <memory>
#include <string>

class SceneManager
{
private:
	std::shared_ptr<TerrainSystem> m_SharedTerrainSystem;
	std::shared_ptr<Camera> m_SharedCamera;

	std::unordered_map<std::string, std::unique_ptr<Scene>> m_Scenes;
	Scene* m_ActiveScene;
	std::string m_ActiveSceneName;

public:
	SceneManager();

	void AddScene(const std::string& name, std::unique_ptr<Scene> scene);
	void RemoveScene(const std::string& name);

	Scene* GetActiveScene() const;
	void SetActiveScene(const std::string& name);

	Scene* GetScene(const std::string& name);

	void Update(float deltaTime);
	void Render();

	~SceneManager();
};