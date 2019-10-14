#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include <sstream>

struct PointLight
{
    glm::vec3 position;
    float offset;
    glm::vec4 color;
};

struct DirectLight
{
    glm::vec3 position;
    float offset1;
    glm::vec3 direction;
    float offset2;
    glm::vec4 color;
};

struct SpotLight
{
    glm::vec3 position;
    float offset;
    glm::vec3 direction;
    float cutoff;
    glm::vec4 color;
};

class Lights
{
private:
    GLuint programID;
    GLuint VAO;

    void loadShader();

public:
    float modelScale;
    std::vector<PointLight*>  pointL;
    std::vector<DirectLight*> directL;
    std::vector<SpotLight*>   spotL;

    Lights();
    ~Lights();

    void addPointLight(const glm::vec3& position = glm::vec3(1.0f), const glm::vec4& color = glm::vec4(1.0f));
    void addDirectLight(const glm::vec3& position = glm::vec3(1.0f), const glm::vec3& direction = glm::vec3(1.0f), const glm::vec4& color = glm::vec4(1.0f));
    void addSpotLight(const glm::vec3& position = glm::vec3(1.0f), const glm::vec3& direction = glm::vec3(1.0f), float cutoff = 0.5f, const glm::vec4& color = glm::vec4(1.0f));

    void drawLights(glm::mat4& view, glm::mat4& perspective);
};