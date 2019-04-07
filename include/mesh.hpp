#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <assimp/Importer.hpp>

#include <string>
#include <vector>
#include <sstream>
#include <stdio.h>

#include "shader.hpp"

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture
{
    GLuint id;
    std::string type;
    aiString path;
};

class Mesh
{
public:
    GLuint VAO, VBO, EBO;
    Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures);
    ~Mesh();
    void draw(Shader *shader);
private:
    void setupMesh();
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;
};