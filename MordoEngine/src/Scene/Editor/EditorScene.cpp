#include "EditorScene.h"
#include "../../Core/Managers/Manager.h"
#include "../../Core/Shader/Shader.h"

EditorScene::EditorScene(std::shared_ptr<terrain::Terrain> terrain, std::shared_ptr<Camera> camera, std::shared_ptr<Renderer> renderer)
	: Scene("terrain"), m_Terrain(terrain), m_Camera(camera), m_Renderer(renderer)
{
	m_CameraController = std::make_unique<EditorCameraController>(m_Camera);
	m_EditorSystem = std::make_unique<EditorSystem>(Manager<Shader>::Get("terrainSelector"));
}


void EditorScene::Update(float deltaTime)
{
	float velocity = 100.0f * m_Terrain->GetWorldScale() * deltaTime;
	m_CameraController->Update(deltaTime, velocity);
	m_EditorSystem->Update(*m_Terrain, *m_Camera);
}

void EditorScene::Render()
{
	glm::mat4 projection = m_Camera->GetProjectionMatrix();
	glm::mat4 view = m_Camera->GetViewMatrix();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, m_EditorSystem->GetWorldPosition());
	glm::vec3 lightDir = glm::vec3(0.0f, 1.0f, 0.0f);

	m_Renderer->Render(m_Camera->GetPosition(), &view, &projection, &model, &lightDir);
	m_EditorSystem->Render(&view, &projection, &model, nullptr);
}

EditorScene::~EditorScene()
{
	//
}