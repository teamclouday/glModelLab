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
    float tt;
} tc;

void main()
{
    vec4 c = texture(material.texture_specular1, TexCoords);
    color = vec4(c.x, c.y * cos(tc.tt / 100), c.z * sin(tc.tt / 100), 1.0);
}