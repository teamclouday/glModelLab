#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform vec3 viewPos;

out VS_OUT
{
    vec2 texCoords;
    vec3 normal;
    vec3 fragPos;
    vec3 viewPos;
} vs_out;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);
    vs_out.texCoords = texCoords;
    vs_out.normal = normal;
    vs_out.fragPos = position;
    vs_out.viewPos = viewPos;
}