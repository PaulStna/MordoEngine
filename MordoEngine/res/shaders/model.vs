#version 330 core

layout(location = 0) in vec3  a_Pos;
layout(location = 1) in vec2  a_TexCoord;
layout(location = 2) in vec3  a_Normal;
layout(location = 3) in ivec4 a_BoneIds;
layout(location = 4) in vec4  a_Weights;

out vec2 TexCoord;
out vec3 Normal;
out vec3 WorldPos;

// Must match MAX_BONES in ModelVertex.h.
const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCES = 4;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec4 plane;

// Only read when skinned is true, so a static model pays nothing for these.
uniform bool skinned;
uniform mat4 finalBones[MAX_BONES];

void main()
{
    vec4 localPos    = vec4(a_Pos, 1.0);
    vec3 localNormal = a_Normal;

    if (skinned)
    {
        mat4 skin  = mat4(0.0);
        float total = 0.0;

        for (int i = 0; i < MAX_BONE_INFLUENCES; ++i)
        {
            // Unused slots are marked with bone -1.
            if (a_BoneIds[i] < 0 || a_BoneIds[i] >= MAX_BONES) continue;

            skin  += finalBones[a_BoneIds[i]] * a_Weights[i];
            total += a_Weights[i];
        }

        // Dividing by the total normalises the blend, so an export whose weights
        // do not add up to 1 still comes out the right size instead of shrinking.
        if (total > 0.0)
        {
            skin /= total;
            localPos    = skin * localPos;
            localNormal = mat3(skin) * localNormal;
        }
    }

    vec4 worldPos = model * localPos;
    gl_Position = projection * view * worldPos;

    // Same clip plane the terrain uses, so models appear in the water
    // reflection and refraction passes.
    gl_ClipDistance[0] = dot(worldPos, plane);

    TexCoord = a_TexCoord;
    WorldPos = worldPos.xyz;
    Normal   = normalize(mat3(model) * localNormal);
}
