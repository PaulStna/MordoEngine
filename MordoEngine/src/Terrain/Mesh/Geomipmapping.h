#pragma once
#include "LodManager.h"
#include "../Terrain.h"
#include "../TerrainVertex.h"
#include "../../Renderer/MeshDrawCall.h"
#include <glm/vec3.hpp>
#include <vector>

class Geomipmapping
{
public:
    Geomipmapping(const terrain::Terrain& terrain, int patchSize);

    void Build(const terrain::Terrain& terrain);
    const std::vector<int>& RebuildModifiedVertices(const terrain::Terrain& terrain);
    const std::vector<terrain::Vertex>& GetVertices() const { return m_Vertices; }
    const std::vector<unsigned int>&    GetIndices()  const { return m_Indices; }
    void SelectLods(const glm::vec3& cameraPos);
    const std::vector<MeshDrawCall>& GetDrawCalls() const { return m_DrawCalls; }

private:
    struct SingleLodInfo {
        int start = 0;
        int count = 0;
    };

    struct LodInfo {
        SingleLodInfo info[2][2][2][2];
    };

    int m_Width = 0;
    int m_Depth = 0;
    int m_PatchSize = 0;
    int m_NumPatchesX = 0;
    int m_NumPatchesZ = 0;
    int m_MaxLOD = 0;

    LodManager m_LodManager;
    std::vector<LodInfo> m_LodInfo;

    std::vector<terrain::Vertex> m_Vertices;
    std::vector<unsigned int>    m_Indices;
    std::vector<MeshDrawCall>    m_DrawCalls;

    // Reused across modification updates so no allocation happens per edit.
    std::vector<int> m_AffectedIndices;

    void ValidateGrid(const terrain::Terrain& terrain);
    void InitVertex(terrain::Vertex& vertex, const terrain::Terrain& terrain,
        std::size_t x, std::size_t z) const;
    void InitVertices(const terrain::Terrain& terrain);
    int  InitIndices();
    void CalculateSmoothNormals();
    void RecalculateNormalAt(int index);
    unsigned int AddTriangle(unsigned int index, unsigned int vertex1,
        unsigned int vertex2, unsigned int vertex3);
    int CalcNumIndices();
    int InitIndicesLOD(int index, int lod);
    int InitIndicesLODSingle(int index, int lodCore, int lodLeft, int lodRight,
        int lodTop, int lodBottom);
    unsigned int CreateTriangleFan(int index, int lodCore, int lodLeft, int lodRight,
        int lodTop, int lodBottom, int x, int z);
    static int Powi(int base, int exp);
};
