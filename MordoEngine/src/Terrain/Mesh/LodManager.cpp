#include "LodManager.h"
#include <glm/glm.hpp>
#include <cmath>
#include <cstdio>
#include <cstdlib>

static constexpr float Z_FAR = 1000.0f;

int LodManager::InitLodManager(int patchSize, int numPatchesX, int numPatchesZ, float worldScale)
{
    m_PatchSize = patchSize;
    m_NumPatchesX = numPatchesX;
    m_NumPatchesZ = numPatchesZ;
    m_WorldScale = worldScale;

    CalcMaxLOD();

    m_Map.resize(m_NumPatchesX * m_NumPatchesZ);
    m_Regions.resize(m_MaxLOD + 1);

    CalcLodRegions();
    return m_MaxLOD;
}

void LodManager::CalcMaxLOD()
{
    int numSegments = m_PatchSize - 1;
    float logValue = std::log2((float)numSegments);

    if (std::ceil(logValue) != std::floor(logValue)) {
        printf("Patch size - 1 must be power of two\n");
        std::exit(1);
    }

    m_MaxLOD = (int)logValue - 1;
}

void LodManager::Update(const glm::vec3& cameraPos)
{
    UpdateLodMapPass1(cameraPos);
    UpdateLodMapPass2();
}

void LodManager::UpdateLodMapPass1(const glm::vec3& cameraPos)
{
    int centerStep = m_PatchSize / 2;

    for (int z = 0; z < m_NumPatchesZ; z++) {
        for (int x = 0; x < m_NumPatchesX; x++) {

            int worldX = x * (m_PatchSize - 1) + centerStep;
            int worldZ = z * (m_PatchSize - 1) + centerStep;

            glm::vec3 patchCenter(
                worldX * m_WorldScale,
                0.0f,
                worldZ * m_WorldScale
            );

            float distance = glm::distance(cameraPos, patchCenter);
            m_Map[Index(x, z)].core = DistanceToLod(distance);
        }
    }
}

void LodManager::UpdateLodMapPass2()
{
    for (int z = 0; z < m_NumPatchesZ; z++) {
        for (int x = 0; x < m_NumPatchesX; x++) {

            PatchLod& patch = m_Map[Index(x, z)];
            int coreLod = patch.core;

            if (x > 0)
                patch.left = (m_Map[Index(x - 1, z)].core > coreLod);

            if (x < m_NumPatchesX - 1)
                patch.right = (m_Map[Index(x + 1, z)].core > coreLod);

            if (z > 0)
                patch.bottom= (m_Map[Index(x, z - 1)].core > coreLod);

            if (z < m_NumPatchesZ - 1)
                patch.top = (m_Map[Index(x, z + 1)].core > coreLod);
        }
    }
}

int LodManager::DistanceToLod(float distance) const
{
    for (int i = 0; i <= m_MaxLOD; i++) {
        if (distance < m_Regions[i])
            return i;
    }
    return m_MaxLOD;
}

const LodManager::PatchLod&
LodManager::GetPatchLod(int patchX, int patchZ) const
{
    return m_Map[Index(patchX, patchZ)];
}

void LodManager::CalcLodRegions()
{
    int sum = 0;
    for (int i = 0; i <= m_MaxLOD; i++)
        sum += (i + 1);

    float step = Z_FAR / sum;
    int accum = 0;

    for (int i = 0; i <= m_MaxLOD; i++) {
        int range = (int)(step * (i + 1));
        accum += range;
        m_Regions[i] = accum;
    }
}
