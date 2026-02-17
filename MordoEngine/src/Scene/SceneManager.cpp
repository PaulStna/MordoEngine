#include "SceneManager.h"
#include "Editor/EditorScene.h"
#include "Game/GameScene.h"
#include "../API/OpenGL/OpenGLBackend.h"
#include "../Terrain/FaultFormationTerrain.h"
#include "../Terrain/MidpointDisplacement.h"
#include "../Renderer/Geomipmapping.h"
#include "../Input/Input.h"
#include "../Core/Managers/Manager.h"
#include "../Core/Shader/Shader.h"
#include <iostream>

SceneManager::SceneManager() : m_ActiveScene(nullptr), m_ActiveSceneName("")
{
	Input::DisableCursor();
	// m_SharedTerrain = std::make_shared<HeightMapTerrain>("res/maps/heightmap.raw");
	//m_SharedTerrain = std::make_unique<MidpointDisplacement>(1057, 3.0f, .8f, 0, terrain::RAW_HEIGHT_MAX);
	m_SharedTerrain = std::make_shared<FaultFormationTerrain>(1057, 3.0f, 50, 0, terrain::RAW_HEIGHT_MAX, 0.15f);
	m_SharedTerrain->SetHeightScale(200.0f * m_SharedTerrain->GetWorldScale());

	m_SharedCamera = std::make_shared<Camera>(glm::vec3{ 0.0f, 50.0f, .80f },
											  OpenGLBackend::SCR_WIDTH, 
		                                      OpenGLBackend::SCR_HEIGHT );
	m_SharedCamera->SetPosition(glm::vec3(m_SharedTerrain->GetSize() / 2,
										  m_SharedTerrain->GetHeightScale(),
										  m_SharedTerrain->GetSize() / 2));
	m_SharedCamera->LookAt(glm::vec3(m_SharedTerrain->GetSize() / 2, 180.0f, m_SharedTerrain->GetSize() / 2));
	
	m_SharedRenderer = std::make_shared<Geomipmapping>(Manager<Shader>::Get("terrain"), * m_SharedTerrain, 33.0f);

	AddScene("game", std::make_unique<GameScene>(m_SharedTerrain, m_SharedCamera, m_SharedRenderer));
	AddScene("editor", std::make_unique<EditorScene>(m_SharedTerrain, m_SharedCamera, m_SharedRenderer));
	SetActiveScene("game");
}

void SceneManager::AddScene(const std::string& name, std::unique_ptr<Scene> scene)
{
	if (m_Scenes.find(name) != m_Scenes.end()) {
		std::cerr << "Scene '" << name << "' already exists!" << std::endl;
		exit(-1);
	}
	m_Scenes[name] = std::move(scene);
}

void SceneManager::RemoveScene(const std::string& name)
{
	if (m_ActiveSceneName == name) {
		m_ActiveScene = nullptr;
		m_ActiveSceneName = "";
	}
	m_Scenes.erase(name);
}

Scene* SceneManager::GetActiveScene() const
{
	return m_ActiveScene;
}

void SceneManager::SetActiveScene(const std::string& name) {
	auto it = m_Scenes.find(name);
	if (it == m_Scenes.end()) {
		std::cerr << "Scene '" << name << "' not found!" << std::endl;
		exit(-1);
	}
	m_ActiveScene = it->second.get();
	m_ActiveSceneName = name;
}

Scene* SceneManager::GetScene(const std::string& name)
{
	auto it = m_Scenes.find(name);
	return (it != m_Scenes.end()) ? it->second.get() : nullptr;
}

void SceneManager::Update(float deltaTime)
{
	if (Input::KeyPressed(GLFW_KEY_E)) {
		SetActiveScene("editor");
	}

	if (Input::KeyPressed(GLFW_KEY_G)) {
		SetActiveScene("game");
	}

	if (m_ActiveScene) {
		m_ActiveScene->Update(deltaTime);
	}
}

void SceneManager::Render()
{
	if (m_ActiveScene) {
		m_ActiveScene->Render();
	}
}

SceneManager::~SceneManager() {
	m_Scenes.clear();
	m_SharedTerrain->UnloadHeightMap();
}