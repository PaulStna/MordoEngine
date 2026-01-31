#pragma once
#include <glad/glad.h>
#include <string>
#include "../../Camera/Camera.h"

class CubeRenderer {
private:
    unsigned int vao, vbo, ebo , instanceVbo;
    const unsigned int chunkSide; 
    const unsigned int totalInstances; 
    glm::vec2 colorCoord;
    std::string defaultShaderID;
    std::string defaultTextureID;
    Camera camera;
    void HandleInputs(float deltaTime);

public:
    CubeRenderer(unsigned int side);
    ~CubeRenderer();

    void Update(float deltaTime);
    void Render();
};