#pragma once
#include <glad/glad.h>
#include "../MeshDrawCall.h"
#include "../../Core/Model/ModelData.h"
#include <vector>

/// The GPU side of a model: one VAO, one vertex buffer and one index buffer
/// holding every submesh of the file back to back.
///
/// Packing them together is what makes drawing a model a run of
/// glDrawElementsBaseVertex calls against a single bound VAO, instead of a bind
/// per piece.
///
/// It owns GL names, so it is neither copyable nor movable — the destructor
/// would delete the same buffers twice. That is why Model holds one through a
/// unique_ptr: the indirection is what keeps Model itself movable.
class ModelRenderer
{
public:
	ModelRenderer();
	ModelRenderer(const ModelRenderer&) = delete;
	ModelRenderer& operator=(const ModelRenderer&) = delete;
	~ModelRenderer();

	/// Uploads every submesh into the shared buffers.
	/// @param model CPU geometry from ModelLoader.
	/// @return One draw call per submesh, in the same order, each already
	///         carrying the base vertex its indices are relative to.
	std::vector<MeshDrawCall> Upload(const ModelData& model);

	/// Issues one indexed draw. Binds this renderer's VAO, so the caller only
	/// has to have set the shader and its uniforms.
	void Render(const MeshDrawCall& drawCall) const;

private:
	GLuint m_Vao = 0;
	GLuint m_Vbo = 0;
	GLuint m_Ebo = 0;

	void CreateGLState();
};
