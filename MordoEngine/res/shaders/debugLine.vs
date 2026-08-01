#version 330 core

layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec3 a_Color;

uniform mat4 projection;
uniform mat4 view;

out vec3 v_Color;

void main()
{
    // No model matrix: debug lines arrive already in world space, since a batch
    // holds segments belonging to many different objects.
    v_Color = a_Color;
    gl_Position = projection * view * vec4(a_Pos, 1.0);
}
