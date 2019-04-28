#version 450 core
struct Material
{
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
};

in vec2 TexCoords;
out vec4 color;

uniform Material material;
layout (binding = 0) uniform myTime
{
    float varSin;
    float varCos;
} tc;

void main()
{
    vec4 c = texture(material.texture_specular1, TexCoords);
    color = vec4(c.x * tc.varSin, c.y * tc.varCos, c.z * tc.varSin, 1.0);
}