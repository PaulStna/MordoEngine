    #pragma once
    #include "PointLightData.h"
    #include "../../Renderer/Cube/CubePointLightRenderer.h"
    #include "../../Renderer/RenderContext.h"
    #include "../../Core/Shader/Shader.h"
    #include <memory>

    class PointLight
    {
    private:
	    PointLightData m_Data;
	    std::unique_ptr<CubePointLightRenderer> m_Renderer;

    public:
	    // debugCube draws the white marker at the light's position. Wanted while
	    // placing lights by hand, not wanted on a lamp that already has a body,
	    // and when it is off no cube geometry is built at all.
	    PointLight(const glm::vec3 position, bool debugCube = true);
        PointLight(const PointLight&) = delete;
        PointLight& operator=(const PointLight&) = delete;

        PointLight(PointLight&& other) noexcept
            : m_Data(std::move(other.m_Data)),
            m_Renderer(std::move(other.m_Renderer))
        {
        }

        PointLight& operator=(PointLight&& other) noexcept
        {
            if (this != &other)
            {
                m_Data = std::move(other.m_Data);
                m_Renderer = std::move(other.m_Renderer);
            }
            return *this;
        }

	    void Update(float deltaTime);
	    // The light owns its transform, so it builds its own model matrix and
	    // ignores renderContext.model.
	    void Render(const Shader& shader, const RenderContext& renderContext);

        void SetPosition(const glm::vec3& position);
	    const PointLightData& GetData() const { return m_Data; }
    };

