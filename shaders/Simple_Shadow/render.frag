#version 330 core
struct Material
{
    sampler2D tex_diff;
    sampler2D tex_emi;
    vec4 diffuse;
    vec4 emissive;
    float shininess;
    int texCount;
};

uniform Material material;
uniform float exposure = 1.0;

in VS_OUT
{
    vec2 texCoords;
    vec3 normal;
    vec3 fragPos;
    vec3 viewPos;
    vec4 shadowCoords;
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
    float coeff;
    vec3 direction;
    float cutOff;
    vec3 color;
    float cutOffOuter;
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

uniform float shadow_enabled = 0.0;
uniform sampler2D depthMap;

vec3 calcDirectL(int index, vec3 originalColor);
vec3 calcPointL(int index, vec3 originalColor);
vec3 calcSpotL(int index, vec3 originalColor);
void calcShadow();

float shadow = 0.0;

void main()
{
    vec4 originalColor = material.diffuse;
    if(material.texCount != 0)
        originalColor = texture(material.tex_diff, fs_in.texCoords);
    vec3 result = originalColor.rgb;
    if(shadow_enabled > 0.5)
        calcShadow();
    if(NUM_DIRECTL > 0 || NUM_POINTL > 0 || NUM_SPOTL > 0)
    {
        vec3 newColor = vec3(0.0, 0.0, 0.0);
        for(int i = 0; i < NUM_DIRECTL; i++)
            newColor += calcDirectL(i, result);
        for(int i = 0; i < NUM_POINTL; i++)
            newColor += calcPointL(i, result);
        for(int i = 0; i < NUM_SPOTL; i++)
            newColor += calcSpotL(i, result);
        result = newColor;
    }
    if(material.texCount == 0)
        result += result*material.emissive.rgb;
    else
        result += result*texture(material.tex_emi, fs_in.texCoords).rgb;
    result = vec3(1.0) - exp(-result * exposure);
    color = vec4(result, originalColor.a);
}

vec3 calcDirectL(int index, vec3 originalColor)
{
    vec3 lightDir = normalize(-lights.directL[index].direction);
    float diff = max(0.0, dot(fs_in.normal, lightDir));
    vec3 reflectDir = reflect(-lightDir, fs_in.normal);
    float spec = pow(max(0.0, dot(normalize(fs_in.viewPos - fs_in.fragPos), reflectDir)), material.shininess);
    vec3 result = 0.05 * originalColor;
    result += (1.0-shadow) * 0.5 * lights.directL[index].color * diff * originalColor;
    result += (1.0-shadow) * 0.8 * lights.directL[index].color * spec * originalColor;
    return result;
}

vec3 calcPointL(int index, vec3 originalColor)
{
    vec3 lightDir = normalize(lights.pointL[index].position - fs_in.fragPos);
    float diff = max(0.0, dot(fs_in.normal, lightDir));
    vec3 reflectDir = reflect(-lightDir, fs_in.normal);
    float spec = pow(max(0.0, dot(normalize(fs_in.viewPos - fs_in.fragPos), reflectDir)), material.shininess);
    float dis = length(lights.pointL[index].position - fs_in.fragPos);
    float att = 1.0 / (1.0 + dis*dis*lights.pointL[index].coeff);
    vec3 result = 0.05 * originalColor;
    result += att * 0.5 * lights.pointL[index].color * diff * originalColor;
    result += att * 0.8 * lights.pointL[index].color * spec * originalColor;
    return result;
}

vec3 calcSpotL(int index, vec3 originalColor)
{
    vec3 lightDir = normalize(lights.spotL[index].position - fs_in.fragPos);
    float diff = max(0.0, dot(fs_in.normal, lightDir));
    vec3 reflectDir = reflect(-lightDir, fs_in.normal);
    float spec = pow(max(0.0, dot(normalize(fs_in.viewPos - fs_in.fragPos), reflectDir)), 32.0);
    float dis = length(lights.spotL[index].position - fs_in.fragPos);
    float att = 1.0 / (1.0 + dis*dis*lights.spotL[index].coeff);
    float theta = degrees(acos(dot(lightDir, normalize(-lights.spotL[index].direction))));
    float epsilon = lights.spotL[index].cutOff - lights.spotL[index].cutOffOuter;
    float intensity = clamp((theta - lights.spotL[index].cutOffOuter) / epsilon, 0.0, 1.0);
    vec3 result = 0.05 * originalColor;
    result += intensity * att * 0.5 * lights.spotL[index].color * diff * originalColor;
    result += intensity * att * 0.8 * lights.spotL[index].color * spec * originalColor;
    return result;
}

void calcShadow()
{
    vec3 projCoords = fs_in.shadowCoords.xyz / fs_in.shadowCoords.w;
    projCoords = projCoords * 0.5 + 0.5;
    if(projCoords.z > 1.0)
    {
        shadow = 0.0;
        return;
    }
    float currentDepth = projCoords.z;
    // float bias = 0.0005;
    float bias = max(0.05 * (1.0 - dot(fs_in.normal, lights.directL[0].direction)), 0.005) / 100.0;
    vec2 texelSize = 1.0 / textureSize(depthMap, 0);
    for(int x = -1; x <= 1; x++)
    {
        for(int y = -1; y <= 1; y++)
        {
            float pcf = texture(depthMap, projCoords.xy + vec2(x, y)*texelSize).r;
            shadow += currentDepth - bias > pcf ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    // float closestDepth = texture(depthMap, projCoords.xy).r;
    // float currentDepth = projCoords.z;
    // shadow = currentDepth - 0.005 > closestDepth ? 1.0 : 0.0;
}