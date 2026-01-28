#pragma once
#include <glad/glad.h>
#include <string>

class Texture {
public:
	GLuint ID;
	Texture(const std::string& texturePath);
	void Use() const;
};
