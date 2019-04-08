#pragma once
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <SOIL.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>
#include <string>
#include <dirent.h>

#include "mesh.hpp"
#include "shader.hpp"

GLuint loadTextureFromFile(const std::string filename, const std::string directory, GLboolean alpha=false);

class Model
{
private:
    std::vector<Mesh*> meshes;
    std::string directory;
    std::vector<Texture> textures_loaded;
public:
    Model(std::string path);
    ~Model();
    void draw(Shader *shader);
    bool exists();
private:
    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh *processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};