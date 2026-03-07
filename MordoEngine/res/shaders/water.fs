#version 330 core

in vec4 ClipSpace;

uniform sampler2D uTexture;

out vec4 FragColor;

void main()
{
    vec2 ndc = ClipSpace.xy / ClipSpace.w;
    vec2 texCoords = ndc * 0.5 + 0.5;

    vec2 reflectCoords = vec2(texCoords.x, 1.0 - texCoords.y);
    reflectCoords = clamp(reflectCoords, 0.001, 0.999);

    FragColor = texture(uTexture, reflectCoords);
}