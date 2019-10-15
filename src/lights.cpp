#include "lights.hpp"
#include <iostream>

Lights::Lights()
{
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, this->UBO);
    glBufferData(GL_UNIFORM_BUFFER, (MAX_LIGHTS*(sizeof(DirectLight)+sizeof(SpotLight)+sizeof(PointLight))), NULL, GL_DYNAMIC_COPY);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    this->modelScale = 0.5f;
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
    if(this->programID)
        glDeleteProgram(this->programID);
    glDeleteVertexArrays(1, &this->VAO);
    glDeleteBuffers(1, &this->UBO);
}

void Lights::loadShader()
{
    // load source
    std::stringstream vertSSTR;
    vertSSTR << "#version 330 core\n"
             << "const vec3[36] vertices = vec3[36](vec3(-1.0f, -1.0f, -1.0f),\n"
             << "                                   vec3(-1.0f, -1.0f,  1.0f),\n"
             << "                                   vec3(-1.0f,  1.0f,  1.0f),\n"
             << "                                   vec3( 1.0f,  1.0f, -1.0f),\n"
             << "                                   vec3(-1.0f, -1.0f, -1.0f),\n"
             << "                                   vec3(-1.0f,  1.0f, -1.0f),\n"
             << "                                   vec3( 1.0f, -1.0f,  1.0f),\n"
             << "                                   vec3(-1.0f, -1.0f, -1.0f),\n"
             << "                                   vec3( 1.0f, -1.0f, -1.0f),\n"
             << "                                   vec3( 1.0f,  1.0f, -1.0f),\n"
             << "                                   vec3( 1.0f, -1.0f, -1.0f),\n"
             << "                                   vec3(-1.0f, -1.0f, -1.0f),\n"
             << "                                   vec3(-1.0f, -1.0f, -1.0f),\n"
             << "                                   vec3(-1.0f,  1.0f,  1.0f),\n"
             << "                                   vec3(-1.0f,  1.0f, -1.0f),\n"
             << "                                   vec3( 1.0f, -1.0f,  1.0f),\n"
             << "                                   vec3(-1.0f, -1.0f,  1.0f),\n"
             << "                                   vec3(-1.0f, -1.0f, -1.0f),\n"
             << "                                   vec3(-1.0f,  1.0f,  1.0f),\n"
             << "                                   vec3(-1.0f, -1.0f,  1.0f),\n"
             << "                                   vec3( 1.0f, -1.0f,  1.0f),\n"
             << "                                   vec3( 1.0f,  1.0f,  1.0f),\n"
             << "                                   vec3( 1.0f, -1.0f, -1.0f),\n"
             << "                                   vec3( 1.0f,  1.0f, -1.0f),\n"
             << "                                   vec3( 1.0f, -1.0f, -1.0f),\n"
             << "                                   vec3( 1.0f,  1.0f,  1.0f),\n"
             << "                                   vec3( 1.0f, -1.0f,  1.0f),\n"
             << "                                   vec3( 1.0f,  1.0f,  1.0f),\n"
             << "                                   vec3( 1.0f,  1.0f, -1.0f),\n"
             << "                                   vec3(-1.0f,  1.0f, -1.0f),\n"
             << "                                   vec3( 1.0f,  1.0f,  1.0f),\n"
             << "                                   vec3(-1.0f,  1.0f, -1.0f),\n"
             << "                                   vec3(-1.0f,  1.0f,  1.0f),\n"
             << "                                   vec3( 1.0f,  1.0f,  1.0f),\n"
             << "                                   vec3(-1.0f,  1.0f,  1.0f),\n"
             << "                                   vec3( 1.0f, -1.0f,  1.0f));\n"
             << "uniform vec3 light_color;\n"
             << "uniform mat4 mvp;\n"
             << "out VS_OUT{\n"
             << "vec3 color;\n"
             << "} vs_out;\n"
             << "void main(){\n"
             << "gl_Position = mvp * vec4(vertices[gl_VertexID], 1.0);\n"
             << "vs_out.color = light_color;\n"
             << "}\n";
    std::string vertSourceStr = vertSSTR.str();
    const char *vertSource = vertSourceStr.c_str();
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &vertSource, NULL);
    glCompileShader(vertShader);

    std::stringstream fragSSTR;
    fragSSTR << "#version 330 core\n"
             << "out vec4 color;\n"
             << "in VS_OUT{\n"
             << "vec3 color;\n"
             << "} fs_in;\n"
             << "void main(){\n"
             << "color = vec4(fs_in.color, 1.0);\n"
             << "}\n";
    std::string fragSourceStr = fragSSTR.str();
    const char *fragSource = fragSourceStr.c_str();
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

void Lights::addDirectLight(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& color)
{
    if(this->directL.size() == MAX_LIGHTS)
        return;
    DirectLight *newLight = new DirectLight;
    newLight->color = color;
    newLight->direction = direction;
    newLight->position = position;
    this->directL.push_back(newLight);
    this->directLFollow.push_back(0);
}

void Lights::addPointLight(const glm::vec3& position, float att, const glm::vec3& color)
{
    if(this->pointL.size() == MAX_LIGHTS)
        return;
    PointLight *newLight = new PointLight;
    newLight->color = color;
    newLight->attenuation = att;
    newLight->position = color;
    this->pointL.push_back(newLight);
}

void Lights::addSpotLight(const glm::vec3& position, float att, const glm::vec3& direction, float cutoff, const glm::vec3& color, float cutoff2)
{
    if(this->spotL.size() == MAX_LIGHTS)
        return;
    SpotLight *newLight = new SpotLight;
    newLight->position = position;
    newLight->attenuation = att;
    newLight->direction = direction;
    newLight->cutoff = cutoff;
    newLight->color = color;
    newLight->cutoff2 = cutoff2;
    this->spotL.push_back(newLight);
    this->spotLFollow.push_back(0);
}

void Lights::drawLights(glm::mat4& view, glm::mat4& perspective)
{
    glBindVertexArray(this->VAO);
    glUseProgram(this->programID);
    for(unsigned i = 0; i < pointL.size(); i++)
    {
        glm::mat4 model(1.0f);
        model = glm::translate(model, pointL[i]->position);
        model = glm::scale(model, glm::vec3(this->modelScale));
        glUniform3fv(glGetUniformLocation(this->programID, "light_color"), 1, &pointL[i]->color[0]);
        glUniformMatrix4fv(glGetUniformLocation(this->programID, "mvp"), 1, GL_FALSE, glm::value_ptr(perspective * view * model));
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    for(unsigned i = 0; i < directL.size(); i++)
    {
        glm::mat4 model(1.0f);
        model = glm::translate(model, directL[i]->position);
        model = glm::scale(model, glm::vec3(this->modelScale));
        glUniform3fv(glGetUniformLocation(this->programID, "light_color"), 1, &directL[i]->color[0]);
        glUniformMatrix4fv(glGetUniformLocation(this->programID, "mvp"), 1, GL_FALSE, glm::value_ptr(perspective * view * model));
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    for(unsigned i = 0; i < spotL.size(); i++)
    {
        glm::mat4 model(1.0f);
        model = glm::translate(model, spotL[i]->position);
        model = glm::scale(model, glm::vec3(this->modelScale));
        glUniform3fv(glGetUniformLocation(this->programID, "light_color"), 1, &spotL[i]->color[0]);
        glUniformMatrix4fv(glGetUniformLocation(this->programID, "mvp"), 1, GL_FALSE, glm::value_ptr(perspective * view * model));
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glUseProgram(0);
    glBindVertexArray(0);
}

void Lights::bind(GLuint program)
{
    GLuint ID = glGetUniformBlockIndex(program, "LIGHTS");
    if(ID == GL_INVALID_INDEX)
        return;
    glUniformBlockBinding(program, ID, 2);
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, this->UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, this->UBO);
    // map point lights first
    PointLight* ptr1 = (PointLight*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, MAX_LIGHTS*sizeof(PointLight), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
    for(unsigned i = 0; i < this->pointL.size(); i++)
    {
        ptr1[i] = *this->pointL[i];
    }
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    // map direct lights next
    DirectLight* ptr2 = (DirectLight*)glMapBufferRange(GL_UNIFORM_BUFFER, MAX_LIGHTS*(sizeof(PointLight)), MAX_LIGHTS*sizeof(DirectLight), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
    for(unsigned i = 0; i < this->directL.size(); i++)
    {
        ptr2[i] = *this->directL[i];
    }
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    // map spot lights last
    SpotLight* ptr3 = (SpotLight*)glMapBufferRange(GL_UNIFORM_BUFFER, MAX_LIGHTS*(sizeof(PointLight)+sizeof(DirectLight)), MAX_LIGHTS*sizeof(SpotLight), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
    for(unsigned i = 0; i < this->spotL.size(); i++)
    {
        ptr3[i] = *this->spotL[i];
    }
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glUniform1i(glGetUniformLocation(program, "NUM_POINTL"), (GLint)this->pointL.size());
    glUniform1i(glGetUniformLocation(program, "NUM_DIRECTL"), (GLint)this->directL.size());
    glUniform1i(glGetUniformLocation(program, "NUM_SPOTL"), (GLint)this->spotL.size());
}