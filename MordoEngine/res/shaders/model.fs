#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 WorldPos;
out vec4 FragColor;

uniform sampler2D texture1;
uniform vec3 viewPos;

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
};
uniform DirLight dirLight;

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

#define MAX_POINT_LIGHTS 16
uniform int numPointLights;
uniform PointLight pointLights[MAX_POINT_LIGHTS];

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 color)
{
    vec3 lightDir = normalize(-light.direction);
    float diff    = max(dot(normal, lightDir), 0.0);
    return light.ambient * color + light.diffuse * diff * color;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 color)
{
    vec3 lightVec  = light.position - fragPos;
    vec3 lightDir  = normalize(lightVec);
    float dist     = length(lightVec);
    float diff     = max(dot(normal, lightDir), 0.0);

    vec3 viewDir    = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec      = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

    float attenuation = 1.0 / (light.constant
                              + light.linear    * dist
                              + light.quadratic * dist * dist);

    return (light.ambient  * color
          + light.diffuse  * diff * color
          + light.specular * spec) * attenuation;
}

void main()
{
    vec3 color = texture(texture1, TexCoord).rgb;
    vec3 norm  = normalize(Normal);

    vec3 result = CalcDirLight(dirLight, norm, color);
    for (int i = 0; i < numPointLights; i++)
        result += CalcPointLight(pointLights[i], norm, WorldPos, color);

    FragColor = vec4(result, 1.0);
}