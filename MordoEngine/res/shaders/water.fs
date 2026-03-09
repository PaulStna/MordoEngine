#version 330 core

in vec4 ClipSpace;
in vec2 TextureCoords;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D dudvMap;
uniform float waveStrength;
uniform float moveFactor;

out vec4 FragColor;

const vec4 BLUE_TINT = vec4(0.0, 0.3, 0.5, 1.0);

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

    FragColor = mix(reflectColor, refractColor, 0.5);
    FragColor = mix(FragColor, BLUE_TINT, 0.2);
}