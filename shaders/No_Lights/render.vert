#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

out VS_OUT
{
    vec2 texCoords;
} vs_out;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);
    vs_out.texCoords = texCoords;
}