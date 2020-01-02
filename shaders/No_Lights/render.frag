#version 330 core
struct Material
{
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
};

uniform Material material;
uniform float material_exists;
uniform float material_alpha;

uniform float exposure = 1.0;

in VS_OUT
{
    vec2 texCoords;
} fs_in;

layout (location = 0) out vec4 color;

void main()
{
    if(material_exists > 0.5)
        color = texture(material.texture_diffuse1, fs_in.texCoords);
    else
        color = vec4(0.4, 0.5, 0.6, 1.0);
    color = vec4(vec3(1.0) - exp(-color.rgb * exposure), material_alpha);
}