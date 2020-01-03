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
} fs_in;

layout (location = 0) out vec4 color;

void main()
{
    if(material.texCount == 0)
    {
        color = material.diffuse;
        color += vec4((color*material.emissive).rgb, 0.0);
    }
    else
    {
        color = texture(material.tex_diff, fs_in.texCoords);
        color += vec4((color*texture(material.tex_emi, fs_in.texCoords)).rgb, 0.0);
    }
    color = vec4(vec3(1.0) - exp(-color.rgb * exposure), color.a);
}