#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include <sstream>

#define MAX_LIGHTS  5

struct PointLight
{
    glm::vec3 position;
    float offset1;
    glm::vec3 color;
    float offset2;
};

struct DirectLight
{
    glm::vec3 position;
    float offset1;
    glm::vec3 direction;
    float offset2;
    glm::vec3 color;
    float offset3;
};

struct SpotLight
{
    glm::vec3 position;
    float offset1;
    glm::vec3 direction;
    float cutoff;
    glm::vec3 color;
    float offset2;
};

class Lights
{
private:
    GLuint programID;
    GLuint VAO;
    GLuint UBO;

    void loadShader();

public:
    float modelScale;
    std::vector<PointLight*>  pointL;
    std::vector<DirectLight*> directL;
    std::vector<SpotLight*>   spotL;

    Lights();
    ~Lights();

    void addPointLight(const glm::vec3& position = glm::vec3(1.0f), const glm::vec3& color = glm::vec3(1.0f));
    void addDirectLight(const glm::vec3& position = glm::vec3(1.0f), const glm::vec3& direction = glm::vec3(1.0f), const glm::vec3& color = glm::vec3(1.0f));
    void addSpotLight(const glm::vec3& position = glm::vec3(1.0f), const glm::vec3& direction = glm::vec3(1.0f), float cutoff = 0.5f, const glm::vec3& color = glm::vec3(1.0f));

    void drawLights(glm::mat4& view, glm::mat4& perspective);
    void bind(GLuint programID);
};