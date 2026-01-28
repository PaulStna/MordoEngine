#include "Engine.h"
#include <random>
#include "API/OpenGL/OpenGLBackend.h"
#include "Input/Input.h"
#include "Renderer/TriangleRenderer.h"
#include "Core/Texture/TextureManager.h"

void Engine::Run()
{
	OpenGLBackend::Init(800, 800);
	Input::Init(OpenGLBackend::GetGLFWwindow());
	TextureManager::Init();
	TriangleRenderer triangleRenderer;

	float lastTime = 0.0f;

	while (!OpenGLBackend::WindowShouldClose())
	{
		float currentTime = glfwGetTime();
		float deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		OpenGLBackend::Update();

		Input::Update();
		if (Input::KeyPressed(GLFW_KEY_ESCAPE)) {
			OpenGLBackend::CloseWindow();
		}

		triangleRenderer.Update(deltaTime);
		triangleRenderer.Render();

		OpenGLBackend::SwapBuffers();
	}

	OpenGLBackend::Terminate();
}