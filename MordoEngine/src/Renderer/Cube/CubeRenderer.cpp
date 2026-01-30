#include "CubeRenderer.h"
#include "../../Input/Input.h"
#include "../../Core/Managers/Manager.h"
#include "../../Core/Texture/Texture.h"
#include "../../Core/Shader/Shader.h"
#include "../../API/OpenGL/OpenGLBackend.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

CubeRenderer::CubeRenderer() : gen(std::random_device{}()),
camera(Camera(glm::vec3{ 0.0f,0.0f,10.0f }, OpenGLBackend::SCR_WIDTH, OpenGLBackend::SCR_HEIGHT)) {

	defaultShaderID = "basic";
	defaultTextureID = "bricks";
	Input::DisableCursor();
	// positions          // texcoords
	float vertices[] = {
		// Front face
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // 0
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // 1
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // 2
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // 3

		// Back face
		 0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // 4
		-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // 5
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // 6
		 0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // 7

		 // Left face
		 -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // 8
		 -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // 9
		 -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, //10
		 -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, //11

		 // Right face
		  0.5f, -0.5f,  0.5f,  0.0f, 0.0f, //12
		  0.5f, -0.5f, -0.5f,  1.0f, 0.0f, //13
		  0.5f,  0.5f, -0.5f,  1.0f, 1.0f, //14
		  0.5f,  0.5f,  0.5f,  0.0f, 1.0f, //15

		  // Top face
		  -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, //16
		   0.5f,  0.5f,  0.5f,  1.0f, 0.0f, //17
		   0.5f,  0.5f, -0.5f,  1.0f, 1.0f, //18
		  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, //19

		  // Bottom face
		  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, //20
		   0.5f, -0.5f, -0.5f,  1.0f, 0.0f, //21
		   0.5f, -0.5f,  0.5f,  1.0f, 1.0f, //22
		  -0.5f, -0.5f,  0.5f,  0.0f, 1.0f  //23
	};
	unsigned int indices[] = {
		// Front
		0, 1, 2,
		2, 3, 0,

		// Back
		4, 5, 6,
		6, 7, 4,

		// Left
		8, 9, 10,
		10, 11, 8,

		// Right
		12, 13, 14,
		14, 15, 12,

		// Top
		16, 17, 18,
		18, 19, 16,

		// Bottom
		20, 21, 22,
		22, 23, 20
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

	// indices
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

CubeRenderer::~CubeRenderer() {
	cubes.clear();
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void CubeRenderer::AddRandomCube() {
	if (!cubes.empty()) {
		cubes.clear();
	}
	static unsigned int gridSize = 5;
	for (unsigned int x = 0; x < gridSize; x++) {
		for (unsigned int z = 0; z < gridSize; z++) {
			cubes.push_back(
				{ glm::vec3(x, 0.0f, z) }
			);
		}
	}
	gridSize += 5;
}

void CubeRenderer::Update(float deltaTime) {
	for (Cube& cube : cubes) {
	//	cube.Update(deltaTime);
	}

	HandleInputs(deltaTime);
}

void CubeRenderer::HandleInputs(float deltaTime) {
	if (Input::KeyPressed(GLFW_KEY_SPACE))
	{
		AddRandomCube();
	}

	glm::vec3 newCameraPosition = camera.GetPosition();
	float velocity = 25.0f * deltaTime;

	if (Input::KeyDown(GLFW_KEY_W)) {
		newCameraPosition += camera.GetForward() * velocity;
	}
	if (Input::KeyDown(GLFW_KEY_A)) {
		newCameraPosition -= camera.GetRight() * velocity;
	}
	if (Input::KeyDown(GLFW_KEY_S)) {
		newCameraPosition -= camera.GetForward() * velocity;
	}
	if (Input::KeyDown(GLFW_KEY_D)) {
		newCameraPosition += camera.GetRight() * velocity;
	}

	if (newCameraPosition != camera.GetPosition()) {
		camera.SetPosition(newCameraPosition);
	}
	camera.ProcessMouseMovement(Input::MouseDeltaX(), Input::MouseDeltaY());
}

void CubeRenderer::Render() {
	Shader& shader = Manager<Shader>::Get(defaultShaderID);
	shader.Use();

	glBindVertexArray(vao);
	glm::mat4 projection = camera.GetProjectionMatrix();
	glm::mat4 view = camera.GetViewMatrix();
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);

	Manager<Texture>::Get(defaultTextureID).Use();
	for (Cube& cube : cubes) {
		cube.Render(shader);
	}
	glBindVertexArray(0);
}
