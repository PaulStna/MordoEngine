#version 330 core

layout(location = 0) in vec2 pos;
layout(location = 1) in float height;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

void main()
{
    vec3 p = vec3(pos.x, height, pos.y);
    gl_Position = projection * view * model * vec4(p,1.0);
}