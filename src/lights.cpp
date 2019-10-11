#include "lights.hpp"

Lights::Lights()
{
    this->modelScale = 1.0f;
    loadShader();
}

Lights::~Lights()
{
    for(unsigned i = 0; i < this->directL.size(); i++)
        delete this->directL[i];
    for(unsigned i = 0; i < this->pointL.size(); i++)
        delete this->pointL[i];
    for(unsigned i = 0; i < this->spotL.size(); i++)
        delete this->spotL[i];
    if(!this->programID)
        glDeleteProgram(this->programID);
}

void Lights::loadShader()
{
    // load source
    std::stringstream vertSSTR;
    vertSSTR << "#version 330 core\n"
             << "const vec3[] pos = vec3[](vec3( 1.0, -1.0, 1.0),\n"
             << "                          vec3( 1.0, -1.0, 1.0),\n"
             << "                          vec3(-1.0,  1.0, 1.0),\n"
             << "                          vec3( 1.0,  1.0, 1.0));\n"
             << "uniform vec4 light_color;\n"
             << "uniform mat4 mvp;\n"
             << "out VS_OUT{\n"
             << "vec4 color;\n"
             << "} vs_out;\n"
             << "void main(){\n"
             << "gl_Position = mvp * vec4(pos, 1.0);\n"
             << "vs_out.color = light_color;\n"
             << "}\n";
    const char *vertSource = vertSSTR.str().c_str();
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &vertSource, NULL);
    glCompileShader(vertShader);

    std::stringstream fragSSTR;
    fragSSTR << "#version 330 core\n"
             << "out vec4 color\n"
             << "in VS_OUT{\n"
             << "vec4 color;\n"
             << "} fs_in;\n"
             << "void main(){\n"
             << "color = fs_in.color;\n"
             << "}\n";
    const char *fragSource = fragSSTR.str().c_str();
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragSource, NULL);
    glCompileShader(fragShader);

    if(!this->programID)
        glDeleteProgram(this->programID);
    this->programID = glCreateProgram();
    glAttachShader(this->programID, vertShader);
    glAttachShader(this->programID, fragShader);
    glLinkProgram(this->programID);

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
}

void Lights::addDirectLight(const glm::vec3& position, const glm::vec3& direction, const glm::vec4& color)
{
    DirectLight *newLight = new DirectLight;
    newLight->color = color;
    newLight->direction = direction;
    newLight->position = position;
    this->directL.push_back(newLight);
}

void Lights::addPointLight(const glm::vec3& position, const glm::vec4& color)
{
    PointLight *newLight = new PointLight;
    newLight->color = color;
    newLight->position = color;
    this->pointL.push_back(newLight);
}

void Lights::addSpotLight(const glm::vec3& position, const glm::vec3& direction, float cutoff, const glm::vec4& color)
{
    SpotLight *newLight = new SpotLight;
    newLight->position = position;
    newLight->direction = direction;
    newLight->cutoff = cutoff;
    newLight->color = color;
    this->spotL.push_back(newLight);
}

void Lights::drawLights(glm::mat4& view, glm::mat4& perspective)
{
    glUseProgram(this->programID);
    for(unsigned i = 0; i < pointL.size(); i++)
    {
        glm::mat4 model(1.0f);
        model = glm::scale(model, glm::vec3(this->modelScale));
        model = glm::translate(model, pointL[i]->position);
        glUniform4fv(glGetUniformLocation(this->programID, "light_color"), 1, &pointL[i]->color[0]);
        glUniformMatrix4fv(glGetUniformLocation(this->programID, "mvp"), 1, GL_FALSE, glm::value_ptr(view * perspective * model));
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);    
    }
    for(unsigned i = 0; i < directL.size(); i++)
    {
        glm::mat4 model(1.0f);
        model = glm::scale(model, glm::vec3(this->modelScale));
        model = glm::translate(model, directL[i]->position);
        glUniform4fv(glGetUniformLocation(this->programID, "light_color"), 1, &directL[i]->color[0]);
        glUniformMatrix4fv(glGetUniformLocation(this->programID, "mvp"), 1, GL_FALSE, glm::value_ptr(view * perspective * model));
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);    
    }
    for(unsigned i = 0; i < spotL.size(); i++)
    {
        glm::mat4 model(1.0f);
        model = glm::scale(model, glm::vec3(this->modelScale));
        model = glm::translate(model, spotL[i]->position);
        glUniform4fv(glGetUniformLocation(this->programID, "light_color"), 1, &spotL[i]->color[0]);
        glUniformMatrix4fv(glGetUniformLocation(this->programID, "mvp"), 1, GL_FALSE, glm::value_ptr(view * perspective * model));
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);    
    }
    glUseProgram(0);
}