#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <filesystem>

class Shader {
public:
	GLuint ID;
	Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	~Shader();
	Shader(const Shader&) = delete;
	Shader(Shader&& other) noexcept;
	Shader& operator=(const Shader&) = delete;
	Shader& operator=(Shader&& other) noexcept;
	void Use() const;
	void SetInt(const std::string& name, int value) const;
	void SetFloat(const std::string& name, float value) const;
	void SetVec2(const std::string& name, const glm::vec2& value) const;
	void SetVec3(const std::string& name, const glm::vec3& value) const;
	void SetVec4(const std::string& name, const glm::vec4& value) const;
	void SetMat4(const std::string& name, const glm::mat4& mat) const;

	// Uploads a whole uniform array in one call. Doing it element by element would
	// mean a glGetUniformLocation per bone per frame.
	void SetMat4Array(const std::string& name, const std::vector<glm::mat4>& mats) const;
private:
	static std::string readFile(const std::filesystem::path& path);
	static GLuint compileShader(GLenum type, const std::string& source);
};
