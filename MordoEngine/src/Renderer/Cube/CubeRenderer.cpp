#include "CubeRenderer.h"
#include "../../Input/Input.h"
#include "../../Core/Managers/Manager.h"
#include "../../Core/Texture/Texture.h"
#include "../../Core/Shader/Shader.h"
#include "../../API/OpenGL/OpenGLBackend.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <random>

CubeRenderer::CubeRenderer(unsigned int side) :
	camera(Camera(glm::vec3{ 0.0f, 0.0f, 10.0f },
		OpenGLBackend::SCR_WIDTH,
		OpenGLBackend::SCR_HEIGHT)),
	colorCoord(glm::vec2{ 0.1f, 0.4f }),
	chunkSide(side),         
	totalInstances(side*side)    
{

	defaultShaderID = "basic";
	defaultTextureID = "atlas";
	Input::DisableCursor();

	float vertices[] = {
		// ================= TOP =================
		// Atlas: 0.0-0.5 , 0.5-1.0
		    -0.5f, 0.5f, -0.5f,  0.0f, 1.0f, 2.0f,
		     0.5f, 0.5f, -0.5f,  0.5f, 1.0f, 2.0f,
		     0.5f, 0.5f,  0.5f,  0.5f, 0.5f, 2.0f,
		    -0.5f, 0.5f,  0.5f,  0.0f, 0.5f, 2.0f,

		// ================= FRONT (SIDE) =================
		// Atlas: 0.5-1.0 , 0.5-1.0
		    -0.5f,   0.5f, 0.5f,  0.5f, 1.0f, 0.0f,
		     0.5f,   0.5f, 0.5f,  1.0f, 1.0f, 0.0f,
		     0.5f,  -0.5f, 0.5f,  1.0f, 0.5f, 0.0f,
		    -0.5f,  -0.5f, 0.5f,  0.5f, 0.5f, 0.0f,

		// ================= BACK (SIDE) =================
		// Atlas: 0.5-1.0 , 0.5-1.0
		    -0.5f,   0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
			 0.5f,   0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
			 0.5f,  -0.5f, -0.5f,  1.0f, 0.5f, 0.0f,
		    -0.5f,  -0.5f, -0.5f,  0.5f, 0.5f, 0.0f,

		// ================= LEFT (SIDE) =================
		// Atlas: 0.5-1.0 , 0.5-1.0
			-0.5f,  0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.0f,
			-0.5f, -0.5f,  0.5f,  0.5f, 0.5f, 0.0f,

		// ================= RIGHT (SIDE) =================
		// Atlas: 0.5-1.0 , 0.5-1.0
			 0.5f,  0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.0f,
			 0.5f, -0.5f,  0.5f,  0.5f, 0.5f, 0.0f,

		// ================= BOTTOM =================
		// Atlas: 0.0-0.5 , 0.0-0.5
			 -0.5f, -0.5f, -0.5f,  0.0f, 0.5f, 0.0f,
			  0.5f, -0.5f, -0.5f,  0.5f, 0.5f, 0.0f,
			  0.5f, -0.5f,  0.5f,  0.5f, 0.0f, 0.0f,
			 -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f
	};


	unsigned int indices[] = {

		// ================= TOP =================
		0, 1, 2,
		2, 3, 0,

		// ================= FRONT =================
		4, 5, 6,
		6, 7, 4,

		// ================= BACK =================
		8, 9, 10,
		10, 11, 8,

		// ================= LEFT =================
		12, 13, 14,
		14, 15, 12,

		// ================= RIGHT =================
		16, 17, 18,
		18, 19, 16,

		// ================= BOTTOM =================
		20, 21, 22,
		22, 23, 20
	};


	std::vector<glm::vec3> translations(totalInstances);
	int index = 0;
	std::mt19937 gen;
	std::uniform_int_distribution<int> dist(0, 1);
	for ( int x = 0; x < chunkSide; x++) {
		for ( int z = 0; z < chunkSide; z++) {
			glm::vec3 translation{ x, dist(gen) , z };
			translations[index++] = translation;
		}
	}

	// Instance Data
	glGenBuffers(1, &instanceVbo);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);
	glBufferData(GL_ARRAY_BUFFER, translations.size() * sizeof(glm::vec3), translations.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// VBO base
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Texcoords
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// FaceID
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// Translations
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(3, 1);

	// Indices
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

CubeRenderer::~CubeRenderer() {
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
}

void CubeRenderer::Update(float deltaTime) {
	HandleInputs(deltaTime);
}

void CubeRenderer::HandleInputs(float deltaTime) {
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
	shader.SetMat4("projection", projection);
	shader.SetMat4("view", view);
	shader.SetVec2("colorCoord", colorCoord);

	glActiveTexture(GL_TEXTURE0);
	Manager<Texture>::Get(defaultTextureID).Use();
	shader.SetInt("texture1", 0);

	// Bind biome colormap 
	glActiveTexture(GL_TEXTURE1);
	Manager<Texture>::Get("colormap").Use();
	shader.SetInt("colorMap", 1);
	
	glDrawElementsInstanced(
		GL_TRIANGLES,
		36,  
		GL_UNSIGNED_INT,
		0,
		totalInstances
	);

	glBindVertexArray(0);
}