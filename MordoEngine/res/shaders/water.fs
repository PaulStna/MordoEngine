#version 330 core

in vec4 ClipSpace;

uniform sampler2D uRefractionTexture;

out vec4 FragColor;

void main()
{
    vec2 ndc = (ClipSpace.xy / ClipSpace.w) * 0.5 + 0.5;
    FragColor = texture(uRefractionTexture, ndc);
}