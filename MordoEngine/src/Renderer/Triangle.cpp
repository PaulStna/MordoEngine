#include "Triangle.h"

Triangle::Triangle(glm::vec3 position, float size, glm::vec3 color, glm::vec3 velocity)
	: position(position), size(size), color(color), velocity(velocity)
{
	textureID = TextureManager::Load("res/textures/bricks.png");
}

void Triangle::Update(float deltaTime) {

}

void Triangle::Render(const Shader& shader) {
	shader.setVec3("iPos", position);
	shader.setFloat("iSize", size);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Triangle::Render(const Shader& shader, TextureID defaultTextureID) {
	if (textureID != defaultTextureID) {
		TextureManager::Get(textureID).Use();
	}
	Render(shader);
}