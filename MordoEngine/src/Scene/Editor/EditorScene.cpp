#include "EditorScene.h"
#include "../../Core/Shader/Shader.h"
#include <glm/gtc/matrix_transform.hpp>

EditorScene::EditorScene(EngineContext& ctx)
	: m_Ctx(ctx)
{
	World& world = m_Ctx.GetWorld();
	m_CameraController = std::make_unique<EditorCameraController>(world.GetCamera());
	m_EditorSystem = std::make_unique<EditorSystem>(m_Ctx.Shaders().Get("terrainSelector"));
	m_EditorInputController = std::make_unique<EditorInputHandlerController>();
}

void EditorScene::Update(float deltaTime)
{
	World& world = m_Ctx.GetWorld();
	float velocity = 100.0f * world.GetTerrain().GetTerrainWorldScale() * deltaTime;
	m_CameraController->Update(deltaTime, velocity);
	m_EditorSystem->Update(world.GetTerrain().GetTerrain(), world.GetCamera());
	m_EditorInputController->Update(*m_EditorSystem, world.GetTerrain());
}

void EditorScene::Render()
{
	World& world = m_Ctx.GetWorld();
	const RenderContext renderContext = world.MakeRenderContext();

	Shader& terrainShader = m_Ctx.Shaders().Get("terrain");
	world.GetTerrain().Render(terrainShader, renderContext);

	// The selector is the one thing here with its own transform, so it gets a
	// copy of the pass context with the model matrix moved to the cursor.
	RenderContext selectorPass = renderContext;
	selectorPass.model = glm::translate(renderContext.model, m_EditorSystem->GetWorldPosition());
	m_EditorSystem->Render(selectorPass);
}

EditorScene::~EditorScene()
{
	//
}