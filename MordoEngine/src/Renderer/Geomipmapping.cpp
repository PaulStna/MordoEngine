#include "Geomipmapping.h"
#include "../Core/Texture/Texture.h"
#include "../Core/Managers/Manager.h"

Geomipmapping::Geomipmapping(Shader& shader, const terrain::Terrain& terrain, int patchSize) : Renderer(shader)
{
    m_PatchSize = patchSize;
    SetTextureScale(50.0f);
    SetHeightThresholds(0.3f, 0.7f);
    CreateGeomipGrid(terrain);
    m_Texture1ID = "grass";
    m_Texture2ID = "dirt";
    m_Texture3ID = "rock";
}

void Geomipmapping::CreateGeomipGrid(const terrain::Terrain& terrain)
{
    m_Width = terrain.GetSize();
    m_Depth = terrain.GetSize();

    if ((m_Width - 1) % (m_PatchSize - 1) != 0) {
        int recommendedWidth = ((m_Width - 1 + m_PatchSize - 1) / (m_PatchSize - 1)) * (m_PatchSize - 1) + 1;
        printf("Error: Width minus 1 (%d) must be divisible by PatchSize minus 1 (%d)\n",
            m_Width - 1, m_PatchSize - 1);
        printf("Try using Width = %d\n", recommendedWidth);
        exit(0);
    }

    if ((m_Depth - 1) % (m_PatchSize - 1) != 0) {
        int recommendedDepth = ((m_Depth - 1 + m_PatchSize - 1) / (m_PatchSize - 1)) * (m_PatchSize - 1) + 1;
        printf("Error: Depth minus 1 (%d) must be divisible by PatchSize minus 1 (%d)\n",
            m_Depth - 1, m_PatchSize - 1);
        printf("Try using Depth = %d\n", recommendedDepth);
        exit(0);
    }

    if (m_PatchSize < 3) {
        printf("Error: The minimum patch size is 3 (given: %d)\n", m_PatchSize);
        exit(0);
    }

    if (m_PatchSize % 2 == 0) {
        printf("Error: Patch size must be an odd number (given: %d)\n", m_PatchSize);
        exit(0);
    }

    m_NumPatchesX = (m_Width - 1) / (m_PatchSize - 1);
    m_NumPatchesZ = (m_Depth - 1) / (m_PatchSize - 1);

    float worldScale = terrain.GetWorldScale();
    m_MaxLOD = m_LodManager.InitLodManager(m_PatchSize, m_NumPatchesX, m_NumPatchesZ, worldScale);
    m_LodInfo.resize(m_MaxLOD + 1);

    CreateGLState();
    PopulateBuffers(terrain);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Geomipmapping::CreateGLState()
{
    glGenVertexArrays(1, &m_Vao);
    glBindVertexArray(m_Vao);

    glGenBuffers(1, &m_Vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);

    glGenBuffers(1, &m_Ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ebo);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(terrain::Vertex),
        (const void*)offsetof(terrain::Vertex, pos));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(terrain::Vertex),
        (const void*)offsetof(terrain::Vertex, texCoord));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(terrain::Vertex),
        (const void*)offsetof(terrain::Vertex, normal));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(terrain::Vertex),
        (const void*)offsetof(terrain::Vertex, height));
}

void Geomipmapping::PopulateBuffers(const terrain::Terrain& terrain)
{
    std::vector<terrain::Vertex> vertices;
    vertices.resize(m_Width * m_Depth);
    InitVertices(terrain, vertices);

    int numIndices = CalcNumIndices();
    std::vector<unsigned int> indices;
    indices.resize(numIndices);

    numIndices = InitIndices(indices);
    CalculateSmoothNormals(vertices, indices);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(),
        &vertices[0], GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * numIndices,
        &indices[0], GL_STATIC_DRAW);
}

int Geomipmapping::CalcNumIndices()
{
    int numQuads = (m_PatchSize - 1) * (m_PatchSize - 1);
    int numIndices = 0;
    int maxPermutationsPerLevel = 16;
    const int indicesPerQuad = 6;

    for (int lod = 0; lod <= m_MaxLOD; lod++) {
        numIndices += numQuads * indicesPerQuad * maxPermutationsPerLevel;
        numQuads /= 4;
    }

    return numIndices;
}

void Geomipmapping::InitVertices(const terrain::Terrain& terrain, std::vector<terrain::Vertex>& vertices)
{
    int index = 0;

    for (int z = 0; z < m_Depth; z++) {
        for (int x = 0; x < m_Width; x++) {
            assert(index < vertices.size());
            vertices[index].InitVertex(terrain, x, z);
            index++;
        }
    }

    assert(index == vertices.size());
}

int Geomipmapping::InitIndices(std::vector<unsigned int>& indices)
{
    int index = 0;

    for (int lod = 0; lod <= m_MaxLOD; lod++) {
        index = InitIndicesLOD(index, indices, lod);
    }

    return index;
}

int Geomipmapping::InitIndicesLOD(int index, std::vector<unsigned int>& indices, int lod)
{
    int totalIndicesForLOD = 0;

    for (int l = 0; l < 2; l++) {
        for (int r = 0; r < 2; r++) {
            for (int t = 0; t < 2; t++) {
                for (int b = 0; b < 2; b++) {
                    m_LodInfo[lod].info[l][r][t][b].start = index;
                    index = InitIndicesLODSingle(index, indices, lod, lod + l, lod + r, lod + t, lod + b);

                    m_LodInfo[lod].info[l][r][t][b].count = index - m_LodInfo[lod].info[l][r][t][b].start;
                    totalIndicesForLOD += m_LodInfo[lod].info[l][r][t][b].count;
                }
            }
        }
    }

    return index;
}

int Geomipmapping::InitIndicesLODSingle(int index, std::vector<unsigned int>& indices, int lodCore, int lodLeft, int lodRight, int lodTop, int lodBottom)
{
    int fanStep = Powi(2, lodCore + 1);
    int endPos = m_PatchSize - 1 - fanStep;

    for (int z = 0; z <= endPos; z += fanStep) {
        for (int x = 0; x <= endPos; x += fanStep) {
            int lLeft = x == 0 ? lodLeft : lodCore;
            int lRight = x == endPos ? lodRight : lodCore;
            int lBottom = z == 0 ? lodBottom : lodCore;
            int lTop = z == endPos ? lodTop : lodCore;

            index = CreateTriangleFan(index, indices, lodCore, lLeft, lRight, lTop, lBottom, x, z);
        }
    }

    return index;
}

unsigned int Geomipmapping::CreateTriangleFan(int index, std::vector<unsigned int>& indices, int lodCore, int lodLeft, int lodRight, int lodTop, int lodBottom, int x, int z)
{
    int stepLeft = Powi(2, lodLeft);
    int stepRight = Powi(2, lodRight);
    int stepTop = Powi(2, lodTop);
    int stepBottom = Powi(2, lodBottom);
    int stepCenter = Powi(2, lodCore);

    unsigned int indexCenter = (z + stepCenter) * m_Width + x + stepCenter;

    unsigned int indexTemp1 = z * m_Width + x;
    unsigned int indexTemp2 = (z + stepLeft) * m_Width + x;

    index = AddTriangle(index, indices, indexCenter, indexTemp1, indexTemp2);

    if (lodLeft == lodCore) {
        indexTemp1 = indexTemp2;
        indexTemp2 += stepLeft * m_Width;

        index = AddTriangle(index, indices, indexCenter, indexTemp1, indexTemp2);
    }

    indexTemp1 = indexTemp2;
    indexTemp2 += stepTop;

    index = AddTriangle(index, indices, indexCenter, indexTemp1, indexTemp2);

    if (lodTop == lodCore) {
        indexTemp1 = indexTemp2;
        indexTemp2 += stepTop;

        index = AddTriangle(index, indices, indexCenter, indexTemp1, indexTemp2);
    }

    indexTemp1 = indexTemp2;
    indexTemp2 -= stepRight * m_Width;

    index = AddTriangle(index, indices, indexCenter, indexTemp1, indexTemp2);

    if (lodRight == lodCore) {
        indexTemp1 = indexTemp2;
        indexTemp2 -= stepRight * m_Width;

        index = AddTriangle(index, indices, indexCenter, indexTemp1, indexTemp2);
    }

    indexTemp1 = indexTemp2;
    indexTemp2 -= stepBottom;

    index = AddTriangle(index, indices, indexCenter, indexTemp1, indexTemp2);

    if (lodBottom == lodCore) {
        indexTemp1 = indexTemp2;
        indexTemp2 -= stepBottom;

        index = AddTriangle(index, indices, indexCenter, indexTemp1, indexTemp2);
    }

    return index;
}

int Geomipmapping::Powi(int base, int exp)
{
    int result = 1;
    for (int i = 0; i < exp; i++) {
        result *= base;
    }
    return result;
}

unsigned int Geomipmapping::AddTriangle(unsigned int index, std::vector<unsigned int>& indices,
    unsigned int vertex1, unsigned int vertex2, unsigned int vertex3)
{
    assert(index < indices.size());
    indices[index++] = vertex1;
    assert(index < indices.size());
    indices[index++] = vertex2;
    assert(index < indices.size());
    indices[index++] = vertex3;

    return index;
}

void Geomipmapping::CalculateSmoothNormals(std::vector<terrain::Vertex>& vertices,
    std::vector<unsigned int>& indices)
{
    for (int z = 0; z < m_Depth - 1; z += (m_PatchSize - 1)) {
        for (int x = 0; x < m_Width - 1; x += (m_PatchSize - 1)) {
            int baseVertex = z * m_Width + x;
            int numIndices = m_LodInfo[0].info[0][0][0][0].count;

            for (int i = 0; i < numIndices; i += 3) {
                unsigned int index0 = baseVertex + indices[i];
                unsigned int index1 = baseVertex + indices[i + 1];
                unsigned int index2 = baseVertex + indices[i + 2];

                glm::vec3 edge1 = vertices[index1].pos - vertices[index0].pos;
                glm::vec3 edge2 = vertices[index2].pos - vertices[index0].pos;

                glm::vec3 triangleNormal = glm::cross(edge1, edge2);
                triangleNormal = glm::normalize(triangleNormal);

                vertices[index0].normal += triangleNormal;
                vertices[index1].normal += triangleNormal;
                vertices[index2].normal += triangleNormal;
            }
        }
    }

    for (unsigned int i = 0; i < vertices.size(); i++) {
        vertices[i].normal = glm::normalize(vertices[i].normal);
    }
}

void Geomipmapping::Render(const glm::vec3& cameraPos, 
                           const glm::mat4* view,
                           const glm::mat4* projection,
                           const glm::mat4* model,
                           const glm::vec3* lightDir)
{
    Renderer::Render(view, projection, model, lightDir);
    m_LodManager.Update(cameraPos);

    p_Shader.SetInt("texture1", 0);
    glActiveTexture(GL_TEXTURE0);
    Manager<Texture>::Get(m_Texture1ID).Use();

    p_Shader.SetInt("texture2", 1);
    glActiveTexture(GL_TEXTURE1);
    Manager<Texture>::Get(m_Texture2ID).Use();

    p_Shader.SetInt("texture3", 2);
    glActiveTexture(GL_TEXTURE2);
    Manager<Texture>::Get(m_Texture3ID).Use();

    p_Shader.SetFloat("textureScale", m_TextureScale);
    p_Shader.SetFloat("heightThreshold1", m_HeightThreshold1);
    p_Shader.SetFloat("heightThreshold2", m_HeightThreshold2);

    glBindVertexArray(m_Vao);

    for (int patchZ = 0; patchZ < m_NumPatchesZ; patchZ++) {
        for (int patchX = 0; patchX < m_NumPatchesX; patchX++) {
            const LodManager::PatchLod& patchLod = m_LodManager.GetPatchLod(patchX, patchZ);
            int c = patchLod.core;
            int l = patchLod.left;
            int r = patchLod.right;
            int t = patchLod.top;
            int b = patchLod.bottom;

            size_t baseIndex = sizeof(unsigned int) * m_LodInfo[c].info[l][r][t][b].start;

            int z = patchZ * (m_PatchSize - 1);
            int x = patchX * (m_PatchSize - 1);
            int baseVertex = z * m_Width + x;

            glDrawElementsBaseVertex(GL_TRIANGLES, m_LodInfo[c].info[l][r][t][b].count,
                GL_UNSIGNED_INT, (void*)baseIndex, baseVertex);
        }
    }

    glBindVertexArray(0);
}

void Geomipmapping::SetTextureScale(float scale)
{
    m_TextureScale = scale;
}

void Geomipmapping::SetHeightThresholds(float threshold1, float threshold2)
{
    m_HeightThreshold1 = threshold1;
    m_HeightThreshold2 = threshold2;
}

Geomipmapping::~Geomipmapping()
{
    glDeleteBuffers(1, &m_Vbo);
    glDeleteBuffers(1, &m_Ebo);
    glDeleteVertexArrays(1, &m_Vao);
}