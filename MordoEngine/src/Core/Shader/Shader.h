#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <filesystem>

class Shader {
public:
	GLuint ID;
	Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	~Shader();
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;
	Shader(Shader&& other) noexcept;
	Shader& operator=(Shader&& other) noexcept;
	void Use() const;
	void SetInt(const std::string& name, int value) const;
	void SetFloat(const std::string& name, float value) const;
	void SetVec2(const std::string& name, const glm::vec2& value) const;
	void SetVec3(const std::string& name, const glm::vec3& value) const;
	void SetMat4(const std::string& name, const glm::mat4& mat) const;
private:
	static std::string readFile(const std::filesystem::path& path);
	static GLuint compileShader(GLenum type, const std::string& source);
};
