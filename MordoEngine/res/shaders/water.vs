#version 330 core

layout(location = 0) in vec2 a_Pos;
layout(location = 1) in vec2 a_TexCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform float yPos;

out vec4 ClipSpace;

void main()
{
    vec4 worldPos = model * vec4(a_Pos.x, yPos, a_Pos.y, 1.0);
    ClipSpace = projection * view * worldPos;
    gl_Position = ClipSpace;
}