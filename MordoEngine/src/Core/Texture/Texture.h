#pragma once
#include <glad/glad.h>
#include <string>
#include <vector>

class Texture {
public:
	GLuint ID;
	Texture(const std::string& texturePath);
	Texture(const Texture&) = delete;
	Texture(Texture&& other) noexcept;
	Texture& operator=(const Texture&) = delete;
	Texture& operator=(Texture&& other) noexcept;
	Texture(const std::string& filePath, const std::vector<std::string>& faces);
	void Use() const;
	void UseCubeMap() const;
	~Texture();
};
