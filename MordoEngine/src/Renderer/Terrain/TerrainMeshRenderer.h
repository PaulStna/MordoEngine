#pragma once
#include <glad/glad.h>
#include "../MeshDrawCall.h"
#include "../../Terrain/TerrainVertex.h"
#include <vector>

/// The GPU side of the terrain: one VAO holding the whole heightmap as a single
/// vertex and index buffer.
///
/// The mesh is uploaded once and then drawn as a list of slices, which is what
/// lets the LOD technique pick a different index range per patch every frame
/// without any geometry being re-sent. It also means editing the terrain does
/// not have to re-upload it: UpdateVertices patches only what the brush
/// touched.
///
/// Owns GL names, so it is neither copyable nor movable.
class TerrainMeshRenderer
{
public:
	TerrainMeshRenderer();
	TerrainMeshRenderer(const TerrainMeshRenderer&) = delete;
	TerrainMeshRenderer& operator=(const TerrainMeshRenderer&) = delete;
	~TerrainMeshRenderer();

	/// Full (re)upload of the mesh. Call once after the terrain is built.
	void Upload(const std::vector<TerrainVertex>& vertices,
		const std::vector<unsigned int>& indices);

	/// Re-uploads just the listed vertices, leaving the rest of the buffer
	/// alone.
	///
	/// One glBufferSubData per modified vertex, which is worth it because a
	/// brush stroke touches a few hundred out of a quarter of a million.
	///
	/// @param vertices        The full vertex array, not just the changed ones;
	///                        the indices below select from it.
	/// @param modifiedIndices Which vertices to re-send, as indices into
	///                        vertices.
	void UpdateVertices(const std::vector<TerrainVertex>& vertices,
		const std::vector<int>& modifiedIndices);

	/// @param drawCalls One slice per patch, as chosen by the LOD technique this
	///                  frame.
	void Render(const std::vector<MeshDrawCall>& drawCalls) const;

private:
	GLuint m_Vao = 0;
	GLuint m_Vbo = 0;
	GLuint m_Ebo = 0;

	void CreateGLState();
};
