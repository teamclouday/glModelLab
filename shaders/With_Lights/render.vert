#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

#define MAX_LIGHTS  5

struct PointLight
{
    vec3 position;
    vec4 color;
};

struct DirectLight
{
    vec3 position;
    vec3 direction;
    vec4 color;
};

struct SpotLight
{
    vec3 position;
    vec3 direction;
    float cutOff;
    vec4 color;
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

out VS_OUT
{
    vec2 texCoords;
} vs_out;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);
    vs_out.texCoords = texCoords;
}