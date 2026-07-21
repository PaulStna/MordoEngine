#version 330 core

in vec4 ClipSpace;
in vec2 TextureCoords;
in vec3 CameraVector;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D dudvMap;
uniform float waveStrength;
uniform float moveFactor;
uniform int underwater;

out vec4 FragColor;

const vec4 BLUE_TINT = vec4(0.0, 0.3, 0.5, 1.0);
const float REFRACTIVE_STRENGHT_FACTOR = 2.0;

void main()
{
    vec2 ndc = ClipSpace.xy / ClipSpace.w;
    vec2 texCoords = ndc * 0.5 + 0.5;

    vec2 reflectCoords = vec2(texCoords.x, 1.0 - texCoords.y);
    vec2 refractCoords = texCoords;

    vec2 distortion1 = texture(dudvMap, vec2(TextureCoords.x + moveFactor, TextureCoords.y)).rg * 2.0f - 1.0;
    distortion1 *= waveStrength;

    vec2 distortion2 = texture(dudvMap, vec2(1.0 - TextureCoords.x + moveFactor, TextureCoords.y + moveFactor)).rg * 2.0 - 1.0;
    distortion2 *= waveStrength;

    vec2 finalDistortion = distortion1 + distortion2;

    reflectCoords = clamp(reflectCoords + finalDistortion, 0.0001, 0.9999);
    refractCoords = clamp(refractCoords + finalDistortion, 0.001, 0.999);

    vec4 reflectColor = texture(reflectionTexture, reflectCoords);
    vec4 refractColor = texture(refractionTexture, refractCoords);

    if (underwater == 1)
    {
        // Seen from below the surface: a translucent blue sheet with a little
        // shimmer. Alpha < 1 lets the submerged terrain show through it.
        vec3 col = mix(reflectColor.rgb, BLUE_TINT.rgb, 0.6);
        FragColor = vec4(col, 0.5);
        return;
    }

    vec3 normalizedViewVector = normalize(CameraVector);
    float refractiveFactor = dot(normalizedViewVector, vec3(0.0, 1.0, 0.0));
    refractiveFactor = pow(refractiveFactor, REFRACTIVE_STRENGHT_FACTOR);

    FragColor = mix(reflectColor, refractColor, refractiveFactor);
    FragColor = mix(FragColor, BLUE_TINT, 0.2);
}