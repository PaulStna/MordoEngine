#pragma once
#include <glad/glad.h>
#include "../Core/Shader/Shader.h"
#include "Triangle.h"
#include "../Core/Texture/Texture.h"
#include <vector>
#include <random>

class TriangleRenderer {
private:
	GLuint vao, vbo;
	std::vector<Triangle> triangles;
	Shader shader;
	void AddRandomTriangle();
	void HandleInputs();
	TextureID defaultTextureID;
	std::mt19937 gen;
public:
	TriangleRenderer();
	~TriangleRenderer();
	void Update(float deltaTime);
	void Render();
};
