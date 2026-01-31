#version 330 core

in vec2 TexCoord;
in float FaceID;

out vec4 FragColor;

uniform sampler2D texture1;     
uniform sampler2D colorMap;     
uniform vec2 colorCoord;

void main()
{
    vec4 texColor = texture(texture1, TexCoord);
    
    if (FaceID > 1.5) {
        vec4 tintColor = texture(colorMap, colorCoord);
        FragColor = texColor * tintColor;
    } else {
        FragColor = texColor;
    }
}