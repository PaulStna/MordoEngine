#pragma once
#include <glad/glad.h>
#include <string>
#include <vector>

/// One texture resident on the GPU, either a 2D image or a cubemap depending on
/// which constructor built it.
///
/// A failed load is reported and survived, not thrown: the object still exists
/// and still owns a GL name, it simply has no image data, so a missing file
/// shows up as a black surface rather than as a crash. Owns a GL texture name,
/// so it is movable but not copyable.
class Texture
{
public:
	GLuint ID;

	/// Loads a 2D image and builds its mipmaps.
	///
	/// The image is flipped vertically on load, because OpenGL's first row is
	/// the bottom one and image files put it at the top. Wrapping is REPEAT, so
	/// this suits a tiling surface directly.
	///
	/// @param texturePath Image file, absolute, as returned by
	///                    FileSystem::getPath. Supports 1, 3 and 4 channel
	///                    images; any other channel count is rejected.
	Texture(const std::string& texturePath);

	/// Loads the six faces of a cubemap.
	///
	/// Unlike the 2D constructor this does not flip: a cubemap's faces are
	/// defined in a convention that already matches GL, and flipping them turns
	/// the sky inside out. Wrapping is CLAMP_TO_EDGE on all three axes so the
	/// seams between faces do not sample across.
	///
	/// @param filePath Directory holding the faces, with its trailing slash.
	/// @param faces    Six file names appended to filePath, in GL's face order:
	///                 right, left, top, bottom, back, front.
	Texture(const std::string& filePath, const std::vector<std::string>& faces);

	~Texture();

	Texture(const Texture&) = delete;
	Texture(Texture&& other) noexcept;
	Texture& operator=(const Texture&) = delete;
	Texture& operator=(Texture&& other) noexcept;

	/// Binds as GL_TEXTURE_2D, into whichever texture unit is active.
	void Use() const;

	/// Binds as GL_TEXTURE_CUBE_MAP. Use this only on a Texture built by the
	/// cubemap constructor.
	void UseCubeMap() const;
};
