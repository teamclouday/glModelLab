#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <SOIL/SOIL.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <vector>
#include <stdio.h>

#ifdef __unix__
#include <dirent.h>
#endif

#ifdef _WIN32
#include <Windows.h>
#endif

#include "shader.hpp"

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture
{
    GLuint id; // will be diffuse texture id if exists
    unsigned texCount;
    float shininess;
    glm::vec4 diffuse;
    glm::vec4 ambient;
    glm::vec4 specular;
    glm::vec4 emissive;
};

GLuint loadTexture(std::string path);
GLuint loadTextureArray(std::vector<std::string> path);
GLuint loadCubeMap(std::vector<std::string> path);

class Mesh
{
public:
    Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, Texture tex);
    ~Mesh();
    void draw(GLuint program);
private:
    void setupMesh();
    GLuint VAO, VBO, EBO;
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    Texture myTexture;
};

class Model
{
private:
    std::string directory;
    std::vector<Mesh*> meshes;
    // std::vector<Texture> textures_loaded;
public:
    Model(std::string path);
    ~Model();
    void draw(GLuint program);
private:
    std::string findModelName(std::string folderPath);
    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh *processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};