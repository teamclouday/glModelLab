#version 450 core
struct Material
{
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
};

#define MAX_LIGHTS 5
struct Light
{
    vec4 position;
    vec3 color;
    float attenuation;
    float ambientCoeff;
    float coneAngle;
};

out vec4 color;

in VS_OUT
{
    vec2 Tex;
    vec3 Normal;
    vec3 FragPos;
    mat4 model;
} fs_in;

uniform Material material;
uniform vec3 CameraPos;
uniform vec3 CameraDir;
uniform int LightNum;

layout (binding = 0) uniform myLights
{
    Light lights[MAX_LIGHTS];
} light;

vec3 applyLight(Light myLight, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    if(LightNum == 0)
        color = texture(material.texture_diffuse1, fs_in.Tex);
    else
    {
        vec3 result = vec3(0.0);
        vec3 viewDirection = normalize(vec3(fs_in.model * vec4(CameraPos, 1.0)) - fs_in.FragPos);
        for(int i = 0; i < LightNum; i++)
            result += applyLight(light.lights[i], normalize(fs_in.Normal), fs_in.FragPos, viewDirection);
        color = vec4(result, 1.0);
    }
    // vec3 gamma = vec3(1.0/2.2);
    // color = vec4(pow(color.xyz, gamma), 1.0);
}

vec3 applyLight(Light myLight, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 myLightPos = vec3(fs_in.model * vec4(myLight.position.xyz, 1.0));
    vec3 lightDir;
    float attenuation = 1.0;
    if(myLight.position.w == 0.0)
    {
        // directional light
        lightDir = normalize(myLightPos);
        attenuation = 1.0;
    }
    else
    {
        // point light
        if(myLight.coneAngle == 0.0)
        {
            lightDir = normalize(myLightPos - fragPos);
            float distanceToLight = length(myLightPos - fragPos);
            attenuation = 1.0 / (1.0 +  myLight.attenuation * distanceToLight);
        }

        // spot light
        if(myLight.coneAngle > 0.0)
        {
            // lightDir = normalize(CameraPos - fragPos);
            // float distanceToLight = length(vec3(fs_in.model * vec4(CameraPos, 1.0)) - fragPos);
            // attenuation = 1.0 / (1.0 +  myLight.attenuation * distanceToLight);
            // float theta = dot(lightDir, normalize(-(vec3(fs_in.model * vec4(CameraPos, 1.0)) - fragPos)));
            // float intensity;
            // if(theta < myLight.coneAngle)
            //     intensity = pow(theta, 20.0);
            // else
            //     intensity = 0;
            // // float intensity = clamp((theta - myLight.coneAngle) / (5.0 - myLight.coneAngle), 0.0, 1.0);
            // attenuation *= intensity;

            lightDir = normalize(vec3(CameraPos - fragPos));
            float distanceToLight = length(CameraPos - fragPos);
            attenuation = 1.0 / (1.0 +  myLight.attenuation * distanceToLight);
            float lightToSurfaceAngle = degrees(acos(dot(-lightDir, normalize(CameraDir))));
            // float intensity;
            // if(lightToSurfaceAngle > myLight.coneAngle)
            //     attenuation = 0.0;
            float intensity = clamp((lightToSurfaceAngle - myLight.coneAngle) / (5.0 - myLight.coneAngle), 0.0, 1.0);
            attenuation *= intensity;
        }
    }

    vec3 ambient = myLight.ambientCoeff * vec3(texture(material.texture_diffuse1, fs_in.Tex));

    float diffuseCoefficient = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = (0.8 * myLight.color) * diffuseCoefficient * vec3(texture(material.texture_diffuse1, fs_in.Tex));
    
    float specularCoefficient = pow(max(dot(viewDir, reflect(-lightDir, normal)), 0.0), 32.0);
    vec3 specular = myLight.color * specularCoefficient * vec3(texture(material.texture_specular1, fs_in.Tex));

    return attenuation * (ambient + diffuse + specular);
}