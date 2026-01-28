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
	void Use() const;
	void setFloat(const std::string& name, float value) const;
	void setVec2(const std::string& name, const glm::vec2& value) const;
	void setVec3(const std::string& name, const glm::vec3& value) const;
private:
	static std::string readFile(const std::filesystem::path& path);
	static GLuint compileShader(GLenum type, const std::string& source);
};
