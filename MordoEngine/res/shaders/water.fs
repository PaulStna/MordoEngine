#version 330 core

in vec4 ClipSpace;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;

out vec4 FragColor;

void main()
{
    vec2 ndc = ClipSpace.xy / ClipSpace.w;
    vec2 texCoords = ndc * 0.5 + 0.5;

    vec2 reflectCoords = vec2(texCoords.x, 1.0 - texCoords.y);
    vec2 refractCoords = texCoords;

    vec4 reflectColor = texture(reflectionTexture, reflectCoords);
    vec4 refractColor = texture(refractionTexture, texCoords);

    FragColor = mix(reflectColor, refractColor, 0.5);
}