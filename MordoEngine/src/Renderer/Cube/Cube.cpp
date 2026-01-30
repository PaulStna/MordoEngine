#include "Cube.h"
#include <glm/gtc/matrix_transform.hpp>
#include "../../Core/Texture/Texture.h"
#include "../../Core/Managers/Manager.h"
Cube::Cube(glm::vec3 position) : Cube(position,
	glm::vec3 {1.0f}, glm::vec3{0.0f}, glm::vec3{0.0f}) {

}

Cube::Cube(glm::vec3 position, glm::vec3 size, glm::vec3 color, glm::vec3 rotationAxe)
	: position(position), size(size), color(color), rotationAxe(rotationAxe)
{
	textureID = "bricks";
}

void Cube::Update(float deltaTime) {
	rotation += angularVelocity * deltaTime;
	if (rotation > 360.0f)
		rotation -= 360.0f;
}

void Cube::Render(const Shader& shader) {
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	model = glm::scale(model, size);
	if ((glm::length(rotationAxe) > 0.0f)) {
		model = glm::rotate(model, glm::radians(rotation), rotationAxe);
	}
	shader.setMat4("model", model);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void Cube::Render(const Shader& shader, const std::string& defaultTextureID) {
	if (textureID != defaultTextureID) {
		Manager<Texture>::Get(textureID).Use();
	}
	Render(shader);
}