#pragma once
#include <glad/glad.h>
#include "../MeshDrawCall.h"
#include "../../Core/Model/ModelData.h"
#include <vector>

class ModelRenderer
{
public:
	ModelRenderer();
	ModelRenderer(const ModelRenderer&) = delete;
	ModelRenderer& operator=(const ModelRenderer&) = delete;
	~ModelRenderer();

	std::vector<MeshDrawCall> Upload(const ModelData& model);

	void Render(const MeshDrawCall& drawCall) const;

private:
	GLuint m_Vao = 0;
	GLuint m_Vbo = 0;
	GLuint m_Ebo = 0;

	void CreateGLState();
};
