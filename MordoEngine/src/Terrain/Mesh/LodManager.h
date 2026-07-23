#pragma once

#include <glm/vec3.hpp>
#include <vector>

class LodManager
{
public:
    struct PatchLod {
        int core = 0;
        int left = 0;
        int right = 0;
        int top = 0;
        int bottom = 0;
    };

    int  InitLodManager(int patchSize, int numPatchesX, int numPatchesZ, float worldScale);
    void Update(const glm::vec3& cameraPos);

    const PatchLod& GetPatchLod(int patchX, int patchZ) const;

private:
    void CalcMaxLOD();
    void CalcLodRegions();

    void UpdateLodMapPass1(const glm::vec3& cameraPos);
    void UpdateLodMapPass2();

    int  DistanceToLod(float distance) const;

    inline int Index(int x, int z) const {
        return z * m_NumPatchesX + x;
    }

private:
    int   m_MaxLOD = 0;
    int   m_PatchSize = 0;
    int   m_NumPatchesX = 0;
    int   m_NumPatchesZ = 0;
    float m_WorldScale = 1.0f;

    std::vector<PatchLod> m_Map;      
    std::vector<int>      m_Regions;  
};
