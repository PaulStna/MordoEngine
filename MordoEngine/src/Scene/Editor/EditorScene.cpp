#include "EditorScene.h"
#include "../../Core/Managers/Manager.h"
#include "../../Core/Shader/Shader.h"
#include "../../Input/Input.h"

EditorScene::EditorScene(std::shared_ptr<terrain::Terrain> terrain, std::shared_ptr<Geomipmapping> renderer, std::shared_ptr<Camera> camera)
	: Scene("terrain"), m_Camera(camera), m_Renderer(renderer), m_Terrain(terrain), 
	  m_TerrainSelector(std::make_unique<AreaSelectorRenderer>(camera))
{
	
}

EditorScene::~EditorScene()
{
	//
}


void EditorScene::Render()
{
	Shader& shader = Manager<Shader>::Get(p_ShaderID);
	shader.Use();

	glm::mat4 projection = m_Camera->GetProjectionMatrix();
	shader.SetMat4("projection", projection);

	glm::mat4 view = m_Camera->GetViewMatrix();
	shader.SetMat4("view", view);

	shader.SetVec3("reverseLightDir", glm::vec3(0.0f, 1.0f, 0.0f));

	m_Renderer->Render(shader, m_Camera->GetPosition());
	m_TerrainSelector->Render();
}

void EditorScene::Update(float deltaTime)
{
	glm::vec3 newCameraPosition = m_Camera->GetPosition();
	float velocity = 100.0f * m_Terrain->GetWorldScale() * deltaTime;

	if (Input::KeyDown(GLFW_KEY_W)) {
		newCameraPosition += m_Camera->GetForward() * velocity;
	}
	if (Input::KeyDown(GLFW_KEY_A)) {
		newCameraPosition -= m_Camera->GetRight() * velocity;
	}
	if (Input::KeyDown(GLFW_KEY_S)) {
		newCameraPosition -= m_Camera->GetForward() * velocity;
	}
	if (Input::KeyDown(GLFW_KEY_D)) {
		newCameraPosition += m_Camera->GetRight() * velocity;
	}

	if (newCameraPosition != m_Camera->GetPosition()) {
		m_Camera->SetPosition(newCameraPosition);
	}
	m_Camera->ProcessMouseMovement(Input::MouseDeltaX(), Input::MouseDeltaY());

	m_TerrainSelector->Update(*m_Terrain);
}