#pragma once
#include "Scene.h"
#include "../Core/EngineContext.h"
#include <unordered_map>
#include <memory>
#include <string>

class SceneManager
{
private:
    EngineContext& m_Ctx;
    std::unordered_map<std::string, std::unique_ptr<Scene>> m_Scenes;
    Scene* m_ActiveScene;
    std::string m_ActiveSceneName;

public:
    SceneManager(EngineContext& ctx);

    void AddScene(const std::string& name, std::unique_ptr<Scene> scene);
    void RemoveScene(const std::string& name);

    Scene* GetActiveScene() const;
    void SetActiveScene(const std::string& name);
    Scene* GetScene(const std::string& name);

    void Update(float deltaTime);
    void Render();

    ~SceneManager();
};