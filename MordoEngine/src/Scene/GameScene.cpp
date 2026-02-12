#include "GameScene.h"
#include "../Terrain/HeightmapTerrain.h"
#include "../Terrain/FaultFormationTerrain.h"
#include "../Terrain/MidpointDisplacement.h"
#include "../API/OpenGL/OpenGLBackend.h"
#include "../Renderer/TriangleRenderer.h"
#include "../Renderer/Geomipmapping.h"
#include "../Core/Managers/Manager.h"
#include "../Core/Shader/Shader.h"
#include "../Input/Input.h"

GameScene::GameScene() : Scene("terrain"),
						//m_Terrain(std::make_shared<HeightMapTerrain>("res/maps/heightmap.raw")),
						m_Terrain(std::make_shared<FaultFormationTerrain>(513, 3.0f, 50, 0, terrain::RAW_HEIGHT_MAX, 0.15f)),
						//m_Terrain(std::make_unique<MidpointDisplacement>(1057, 3.0f, 1.0f, 0, terrain::RAW_HEIGHT_MAX)),
						m_Sun(std::make_unique<Sun>(0.9f)),
						m_Camera(std::make_shared<Camera>(glm::vec3{ 0.0f, 50.0f, .80f },
							OpenGLBackend::SCR_WIDTH,
							OpenGLBackend::SCR_HEIGHT))
{
	m_Terrain->SetHeightScale(200.0f * m_Terrain->GetWorldScale());
	m_Camera->SetPosition(glm::vec3(m_Terrain->GetSize()/2,
								   m_Terrain->GetHeightScale(), 
		                           m_Terrain->GetSize() / 2));
	m_Camera->LookAt(glm::vec3(m_Terrain->GetSize() / 2, 180.0f, m_Terrain->GetSize() / 2));
	//m_Render = std::make_unique<TriangleRenderer>(*m_Terrain);
	m_Render = std::make_shared<Geomipmapping>(*m_Terrain, 33.0f);
	Input::DisableCursor();
}

void GameScene::Update(float deltaTime)
{
	m_Sun->Update(deltaTime);
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
}

void GameScene::Render()
{
	Shader& shader = Manager<Shader>::Get(p_ShaderID);
	shader.Use();

	glm::mat4 projection = m_Camera->GetProjectionMatrix();
	shader.SetMat4("projection", projection);

	glm::mat4 view = m_Camera->GetViewMatrix();
	shader.SetMat4("view", view);

	shader.SetVec3("reverseLightDir", m_Sun->GetReverseLightDirection());
	m_Render->Render(shader, m_Camera->GetPosition());

}

GameScene::~GameScene()
{
	m_Terrain->UnloadHeightMap();
}