#pragma once
#include "../Core/Shader/Shader.h"
#include <glm/vec3.hpp>

class Renderer
{
protected:
	Shader& p_Shader;

public:
	Renderer(Shader& shader) : p_Shader(shader) {}
    virtual void Render(const glm::vec3& cameraPos) {};
    virtual void Render(const glm::vec3& cameraPos,
                        const glm::mat4* view,
                        const glm::mat4* projection,
                        const glm::mat4* model,
                        const glm::vec3* lightDir) {};

    virtual void Render(const glm::mat4* view,
                        const glm::mat4* projection,
                        const glm::mat4* model,
                        const glm::vec3* lightDir)
    {
        p_Shader.Use();
        if (view) {
            p_Shader.SetMat4("view", *view);
        }
        if (projection) {
            p_Shader.SetMat4("projection", *projection);
        }
        if (model) {
            p_Shader.SetMat4("model", *model);
        }
        if (lightDir) {
            p_Shader.SetVec3("reverseLightDir", *lightDir);
        }
    }
    virtual void SetHeights(const std::vector<float>& heights, const glm::vec3& position) {};
	virtual ~Renderer() = default;
};
