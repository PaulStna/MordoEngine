#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D sceneTexture;
uniform sampler2D dudvMap;
uniform int underwater;
uniform float time;
uniform float submergence; // 0 near the surface .. 1 deep down

const vec3 WATER_COLOR = vec3(0.0, 0.32, 0.45);

void main()
{
    if (underwater == 0)
    {
        // Above the surface: pass the scene through untouched.
        FragColor = vec4(texture(sceneTexture, TexCoords).rgb, 1.0);
        return;
    }

    // Animated wobble driven by the same dudv map the water surface uses, so the
    // whole screen ripples like you are looking through moving water.
    float move = time * 0.02;
    vec2 d1 = texture(dudvMap, vec2(TexCoords.x + move, TexCoords.y)).rg * 2.0 - 1.0;
    vec2 d2 = texture(dudvMap, vec2(-TexCoords.x + move, TexCoords.y + move)).rg * 2.0 - 1.0;
    vec2 distortion = (d1 + d2) * 0.006;

    vec2 uv = clamp(TexCoords + distortion, 0.001, 0.999);
    vec3 color = texture(sceneTexture, uv).rgb;

    // Blue-green tint, stronger the deeper the camera is (murk / fog).
    float tint = mix(0.25, 0.6, submergence);
    color = mix(color, WATER_COLOR, tint);

    // Soft vignette so the view feels enclosed and hazy.
    float vignette = smoothstep(1.1, 0.35, length(TexCoords - vec2(0.5)));
    color = mix(WATER_COLOR * 0.35, color, vignette);

    FragColor = vec4(color, 1.0);
}
