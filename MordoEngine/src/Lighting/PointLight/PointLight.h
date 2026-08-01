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

        // A light that is off keeps everything it had: its colours, its
        // attenuation and its place. LightSystem skips it when filling the
        // shader slots, and Render skips its debug cube, so turning it back on
        // is one bool and not a rebuild.
        bool m_Enabled = true;

    public:
	    // debugCube draws the white marker at the light's position. Wanted while
	    // placing lights by hand, not wanted on a lamp that already has a body,
	    // and when it is off no cube geometry is built at all.
	    PointLight(const glm::vec3 position, bool debugCube = true);
        PointLight(const PointLight&) = delete;
        PointLight& operator=(const PointLight&) = delete;

        PointLight(PointLight&& other) noexcept
            : m_Data(std::move(other.m_Data)),
            m_Renderer(std::move(other.m_Renderer)),
            m_Enabled(other.m_Enabled)
        {
        }

        PointLight& operator=(PointLight&& other) noexcept
        {
            if (this != &other)
            {
                m_Data = std::move(other.m_Data);
                m_Renderer = std::move(other.m_Renderer);
                m_Enabled = other.m_Enabled;
            }
            return *this;
        }

	    void Update(float deltaTime);
	    // The light owns its transform, so it builds its own model matrix and
	    // ignores renderContext.model.
	    void Render(const Shader& shader, const RenderContext& renderContext);

        void SetPosition(const glm::vec3& position);
	    const PointLightData& GetData() const { return m_Data; }

        void SetEnabled(bool enabled) { m_Enabled = enabled; }
        bool IsEnabled() const { return m_Enabled; }
    };

