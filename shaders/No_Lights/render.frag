#version 330 core
struct Material
{
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_ambient1;
};

uniform Material material;

in VS_OUT
{
    vec2 texCoords;
} fs_in;

layout (location = 0) out vec4 color;

void main()
{
    color = texture(material.texture_diffuse1, fs_in.texCoords);
}