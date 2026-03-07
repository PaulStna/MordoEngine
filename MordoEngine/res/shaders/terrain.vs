#version 330 core

layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec3 a_Normal;
layout(location = 3) in float a_Height;

out vec2 TexCoord;
out float Height;
out vec3 Normal;
out vec3 WorldPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec4 plane;
uniform float textureScale;

void main()
{
    vec4 worldPos = model * vec4(a_Pos, 1.0);
    gl_Position = projection * view * worldPos;
    
    gl_ClipDistance[0] = dot(worldPos, plane);

    TexCoord = a_TexCoord * textureScale;
    Height = a_Height;
    WorldPos = worldPos.xyz;
    Normal = normalize(mat3(model) * a_Normal);
}