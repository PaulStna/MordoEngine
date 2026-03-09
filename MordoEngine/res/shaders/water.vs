#version 330 core

layout(location = 0) in vec2 a_Pos;
layout(location = 1) in vec2 a_TexCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform float yPos;

out vec4 ClipSpace;
out vec2 TextureCoords;

const float TEXTURE_SCALE = 6.0;

void main()
{
    vec4 worldPos = model * vec4(a_Pos.x, yPos, a_Pos.y, 1.0);
    ClipSpace = projection * view * worldPos;
    gl_Position = ClipSpace;
    TextureCoords = vec2(a_Pos.x * 0.5 + 0.5, a_Pos.y * 0.5 + 0.5) * TEXTURE_SCALE;
}