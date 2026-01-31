#include "Shader.h"
#include "../FileSystem/FileSystem.h"
#include <fstream>
#include <iostream>


Shader::Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
	std::string vertexCode = readFile(vertexShaderPath);
	std::string fragmentCode = readFile(fragmentShaderPath);

	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexCode);
	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentCode);

	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);

	GLint success;
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[1024];
		glGetProgramInfoLog(ID, 1024, nullptr, infoLog);
		std::cerr << "ERROR::SHADER::PROGRAM_LINKING_FAILED\n"
			<< infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

Shader::~Shader() {
	glDeleteProgram(ID);
}

void Shader::Use() const {
	glUseProgram(ID);
}

std::string Shader::readFile(const std::filesystem::path& path) {
	std::ifstream file(path);

	if (!file.is_open()) {
		std::cerr << "ERROR::SHADER::FILE_NOT_FOUND\n"
			<< path << std::endl;
		return "";
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

GLuint Shader::compileShader(GLenum type, const std::string& source) {
	GLuint shader = glCreateShader(type);
	const char* src = source.c_str();

	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[1024];
		glGetShaderInfoLog(shader, 1024, nullptr, infoLog);

		std::cerr << "ERROR::SHADER::COMPILATION_FAILED ("
			<< (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT")
			<< ")\n" << infoLog << std::endl;
	}

	return shader;
}

void Shader::SetInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::SetFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::SetVec2(const std::string& name, const glm::vec2& value) const
{
	glUniform2fv(glGetUniformLocation(ID, name.c_str()),1,&value[0]);
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value) const
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::SetMat4(const std::string& name, const glm::mat4& mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

Shader::Shader(Shader&& other) noexcept
	: ID(other.ID)
{
	other.ID = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept {
	if (this != &other) {
		if (ID != 0) {
			glDeleteProgram(ID);
		}
		ID = other.ID;
		other.ID = 0;
	}
	return *this;
}