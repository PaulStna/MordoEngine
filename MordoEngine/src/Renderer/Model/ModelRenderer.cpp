#include "ModelRenderer.h"

ModelRenderer::ModelRenderer()
{
	CreateGLState();
	glBindVertexArray(0);
}

void ModelRenderer::CreateGLState()
{
	glGenVertexArrays(1, &m_Vao);
	glBindVertexArray(m_Vao);

	glGenBuffers(1, &m_Vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);

	glGenBuffers(1, &m_Ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ebo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ModelVertex),
		(const void*)offsetof(ModelVertex, pos));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ModelVertex),
		(const void*)offsetof(ModelVertex, texCoord));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(ModelVertex),
		(const void*)offsetof(ModelVertex, normal));

	// Bone indices go through glVertexAttribIPointer, not the usual one: they must
	// reach the shader as real integers. Sent through glVertexAttribPointer they
	// would be converted to float and index the wrong bones.
	glEnableVertexAttribArray(3);
	glVertexAttribIPointer(3, 4, GL_INT, sizeof(ModelVertex),
		(const void*)offsetof(ModelVertex, boneIds));

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(ModelVertex),
		(const void*)offsetof(ModelVertex, weights));
}

std::vector<MeshDrawCall> ModelRenderer::Upload(const ModelData& model)
{
	std::vector<ModelVertex>  vertices;
	std::vector<unsigned int> indices;
	std::vector<MeshDrawCall> drawCalls;
	drawCalls.reserve(model.subMeshes.size());

	for (const SubMeshData& subMesh : model.subMeshes)
	{
		MeshDrawCall drawCall;
		drawCall.indexOffset = static_cast<int>(indices.size());
		drawCall.indexCount = static_cast<int>(subMesh.indices.size());
		drawCall.baseVertex = static_cast<int>(vertices.size());
		drawCalls.push_back(drawCall);

		vertices.insert(vertices.end(), subMesh.vertices.begin(), subMesh.vertices.end());
		indices.insert(indices.end(), subMesh.indices.begin(), subMesh.indices.end());
	}

	glBindVertexArray(m_Vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
	glBufferData(GL_ARRAY_BUFFER,
		vertices.size() * sizeof(ModelVertex), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
	return drawCalls;
}

void ModelRenderer::Render(const MeshDrawCall& drawCall) const
{
	glBindVertexArray(m_Vao);
	glDrawElementsBaseVertex(
		GL_TRIANGLES,
		drawCall.indexCount,
		GL_UNSIGNED_INT,
		(const void*)(drawCall.indexOffset * sizeof(unsigned int)),
		drawCall.baseVertex);
	glBindVertexArray(0);
}

ModelRenderer::~ModelRenderer()
{
	glDeleteBuffers(1, &m_Ebo);
	glDeleteBuffers(1, &m_Vbo);
	glDeleteVertexArrays(1, &m_Vao);
}
