#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <filesystem>

/// One linked GLSL program, plus the typed setters for its uniforms.
///
/// The constructor reads, compiles and links, so a Shader that exists is a
/// Shader you can Use. It owns a GL program name and is therefore movable but
/// not copyable: a copy would delete the same program twice.
class Shader
{
public:
	GLuint ID;

	/// @param vertexShaderPath   Vertex stage source, an absolute path as
	///                           returned by FileSystem::getPath.
	/// @param fragmentShaderPath Fragment stage source, same.
	Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	~Shader();

	Shader(const Shader&) = delete;
	Shader(Shader&& other) noexcept;
	Shader& operator=(const Shader&) = delete;
	Shader& operator=(Shader&& other) noexcept;

	/// Makes this the active program. Every setter below writes into whichever
	/// program is active, so this comes first.
	void Use() const;

	/// @param name Uniform name as spelled in the shader source. A name that is
	///             not there, or was optimised out for being unused, is ignored
	///             rather than reported.
	void SetInt(const std::string& name, int value) const;
	void SetFloat(const std::string& name, float value) const;
	void SetVec2(const std::string& name, const glm::vec2& value) const;
	void SetVec3(const std::string& name, const glm::vec3& value) const;
	void SetVec4(const std::string& name, const glm::vec4& value) const;
	void SetMat4(const std::string& name, const glm::mat4& mat) const;

	/// Uploads a whole uniform array in one call.
	/// @param mats Written to array element 0 upwards. Doing this element by
	///             element would cost a glGetUniformLocation per bone per frame.
	void SetMat4Array(const std::string& name, const std::vector<glm::mat4>& mats) const;

private:
	static std::string readFile(const std::filesystem::path& path);
	static GLuint compileShader(GLenum type, const std::string& source);
};
