#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in float aFaceID;
layout (location = 3) in vec3 aOffSet;

out vec2 TexCoord;
out float FaceID;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * vec4(aPos + aOffSet, 1.0);
    TexCoord = aTexCoord;
    FaceID = aFaceID;
}