#include "TriangleRenderer.h"
#include "../Input/Input.h"
#include <iostream>

TriangleRenderer::TriangleRenderer() : shader("res/shaders/triangle.vs", "res/shaders/triangle.fs"), gen(std::random_device{}()) {

	defaultTextureID = TextureManager::Load("res/textures/bricks.png");
	// base triangle // texcoords
	float vertices[] = {
		 0.0f,  0.5f, 0.0f, 0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f, 1.0f, 1.0f
	};

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// VBO base
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// texcoords
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

TriangleRenderer::~TriangleRenderer() {
	triangles.clear();
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void TriangleRenderer::AddRandomTriangle() {
	std::uniform_real_distribution<float> pos(-1.0f, 1.0f);
	std::uniform_real_distribution<float> sizeDist(0.1f, 0.5f);
	std::uniform_real_distribution<float> color(0.0f, 1.0f);
	std::uniform_real_distribution<float> vel(-0.5f, 0.5f);

	triangles.push_back({
		glm::vec3(pos(gen), pos(gen), 0.0f),
		sizeDist(gen),
		glm::vec3(color(gen), color(gen), color(gen)),
		glm::vec3(vel(gen), vel(gen), 0.0f)
		});
}

void TriangleRenderer::Update(float deltaTime) {
	for (Triangle& triangle : triangles) {
		triangle.Update(deltaTime);
	}
	HandleInputs();
}

void TriangleRenderer::HandleInputs() {
	if (Input::KeyPressed(GLFW_KEY_SPACE))
	{
		AddRandomTriangle();
	}
}

void TriangleRenderer::Render() {
	shader.Use();
	glBindVertexArray(vao);

	TextureManager::Get(defaultTextureID).Use();

	for (Triangle& triangle : triangles) {
		triangle.Render(shader);
	}
	glBindVertexArray(0);
}
