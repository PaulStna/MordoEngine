#pragma once
#include <glm/glm.hpp>
#include "../../Camera/Camera.h"
#include "../../Core/Shader/Shader.h"

class Cube {
public:
	void Update(float deltaTime);
	void Render(const Shader& shader);
	void Render(const Shader& shader, const std::string& defaultTextureID);
	Cube(glm::vec3 position);
	Cube(glm::vec3 position, glm::vec3 size, glm::vec3 color, glm::vec3 rotationAxes);
	std::string textureID;
	float rotation = 0.0f;         
	float angularVelocity = 90.0f;  
private:
	glm::vec3 position;
	glm::vec3 size;
	glm::vec3 color;
	glm::vec3 rotationAxe;
};