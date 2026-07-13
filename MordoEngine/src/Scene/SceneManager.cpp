#include "SceneManager.h"
#include "Editor/EditorScene.h"
#include "Game/GameScene.h"
#include "../Input/Input.h"
#include <iostream>

SceneManager::SceneManager(EngineContext& ctx)
    : m_Ctx(ctx), m_ActiveScene(nullptr), m_ActiveSceneName("")
{
    Input::DisableCursor();
    AddScene("game", std::make_unique<GameScene>(m_Ctx));
    AddScene("editor", std::make_unique<EditorScene>(m_Ctx));
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

void SceneManager::SetActiveScene(const std::string& name)
{
    auto it = m_Scenes.find(name);
    if (it == m_Scenes.end()) {
        std::cerr << "Scene '" << name << "' not found!" << std::endl;
        exit(-1);
    }
    m_ActiveScene = it->second.get();
    m_ActiveScene->OnEntry();
    m_ActiveSceneName = name;
}

Scene* SceneManager::GetScene(const std::string& name)
{
    auto it = m_Scenes.find(name);
    return (it != m_Scenes.end()) ? it->second.get() : nullptr;
}

void SceneManager::Update(float deltaTime)
{
    if (Input::KeyPressed(GLFW_KEY_E)) SetActiveScene("editor");
    if (Input::KeyPressed(GLFW_KEY_G)) SetActiveScene("game");

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

SceneManager::~SceneManager()
{
    m_Scenes.clear();
}