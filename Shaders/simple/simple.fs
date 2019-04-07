#version 330 core
struct Material
{
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
};

in vec2 TexCoords;
out vec4 color;

uniform Material material;

void main()
{
    //color = texture(material.texture_specular1, TexCoords);
    color = vec4(0.5);
}