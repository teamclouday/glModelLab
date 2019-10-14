#version 330 core
struct Material
{
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
};

uniform Material material;
uniform float material_exists;

in VS_OUT
{
    vec2 texCoords;
    vec3 normal;
    vec3 fragPos;
    vec3 viewPos;
} fs_in;

layout (location = 0) out vec4 color;

#define MAX_LIGHTS  5

struct PointLight
{
    vec3 position;
    float coeff;
    vec3 color;
};

struct DirectLight
{
    vec3 position;
    vec3 direction;
    vec3 color;
};

struct SpotLight
{
    vec3 position;
    vec3 direction;
    float cutOff;
    vec3 color;
};

layout (std140) uniform LIGHTS
{
    PointLight[MAX_LIGHTS]  pointL;
    DirectLight[MAX_LIGHTS] directL;
    SpotLight[MAX_LIGHTS]   spotL;
} lights;

uniform int NUM_POINTL  = 0;
uniform int NUM_DIRECTL = 0;
uniform int NUM_SPOTL   = 0;

vec3 calcDirectL(int index, vec3 originalColor);
vec3 calcPointL(int index, vec3 originalColor);

void main()
{
    vec3 result = vec3(texture(material.texture_diffuse1, fs_in.texCoords));
    if(material_exists < 0.5)
        result = vec3(0.4, 0.5, 0.6);
    if(NUM_DIRECTL != 0 || NUM_POINTL != 0 || NUM_SPOTL != 0)
    {
        vec3 newColor = vec3(0.0, 0.0, 0.0);
        for(int i = 0; i < NUM_DIRECTL; i++)
            newColor += calcDirectL(i, result);
        for(int i = 0; i < NUM_POINTL; i++)
            newColor += calcPointL(i, result);
        result = newColor;
    }
    color = vec4(result, 1.0);
}

vec3 calcDirectL(int index, vec3 originalColor)
{
    vec3 lightDir = normalize(-lights.directL[index].direction);
    float diff = max(0.0, dot(fs_in.normal, lightDir));
    vec3 reflectDir = reflect(-lightDir, fs_in.normal);
    float spec = pow(max(0.0, dot(normalize(fs_in.viewPos - fs_in.fragPos), reflectDir)), 32.0);
    vec3 result = 0.1 * originalColor;
    result += 0.5 * lights.directL[index].color * diff * originalColor;
    result += 0.9 * lights.directL[index].color * spec * originalColor;
    return result;
}

vec3 calcPointL(int index, vec3 originalColor)
{
    vec3 lightDir = normalize(lights.pointL[index].position - fs_in.fragPos);
    float diff = max(0.0, dot(fs_in.normal, lightDir));
    vec3 reflectDir = reflect(-lightDir, fs_in.normal);
    float spec = pow(max(0.0, dot(normalize(fs_in.viewPos - fs_in.fragPos), reflectDir)), 32.0);
    float dis = length(lights.pointL[index].position - fs_in.fragPos);
    float att = 1.0 / (1.0 + dis*dis*lights.pointL[index].coeff);
    vec3 result = 0.1 * originalColor;
    result += att * 0.5 * lights.pointL[index].color * diff * originalColor;
    result += att * 0.9 * lights.pointL[index].color * spec * originalColor;
    return result;
}