#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out VS_OUT
{
    vec2 Tex;
    vec3 Normal;
    vec3 FragPos;
} vs_out;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);
    vs_out.Tex = texCoords;
    vs_out.Normal = normalize(mat3(transpose(inverse(model))) * normal);
    vs_out.FragPos = vec3(model * vec4(position, 1.0));
}