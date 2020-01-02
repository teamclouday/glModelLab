#version 330 core

struct Material
{
    sampler2D tex;
    vec4 diffuse;
    vec4 ambient;
    vec4 specular;
    vec4 emissive;
    float shininess;
    int texCount;
};

uniform Material material;
uniform float exposure = 1.0;

in VS_OUT
{
    vec2 texCoords;
} fs_in;

layout (location = 0) out vec4 color;

void main()
{
    if(material.texCount == 0)
        color = material.diffuse;
    else
        color = texture(material.tex, fs_in.texCoords);
    color = vec4(vec3(1.0) - exp(-color.rgb * exposure), color.a);
}