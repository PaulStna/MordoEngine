#pragma once
#include "../Core/Texture/TextureManager.h"
#include "../Core/Shader/Shader.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

class Triangle {
public:
	void Update(float deltaTime);
	void Render(const Shader& shader);
	void Render(const Shader& shader, TextureID defaultTextureID);
	Triangle(glm::vec3 position, float size, glm::vec3 color, glm::vec3 velocity);
	TextureID textureID;
	glm::vec3 getPosition() const { return position; }
private:
	glm::vec3 position;
	float size;
	glm::vec3 color;
	glm::vec3 velocity;
};
