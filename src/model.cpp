#include "model.hpp"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    this->setupMesh();
}

Mesh::~Mesh()
{
    glDeleteBuffers(1, &this->VBO);
    glDeleteBuffers(1, &this->EBO);
    glDeleteVertexArrays(1, &this->VAO);
    this->vertices.clear();
    this->vertices.shrink_to_fit();
    this->indices.clear();
    this->indices.shrink_to_fit();
    this->textures.clear();
    this->textures.shrink_to_fit();
}

void Mesh::setupMesh()
{
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glGenBuffers(1, &this->EBO);

    glBindVertexArray(this->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
}

void Mesh::draw(GLuint program)
{
    // name convensions:
    // diffuse texture named: texture_diffuseN
    // specular texture named: texture_specularN

    GLuint diffuseNr = 1;
    GLuint specularNr = 1;
    GLuint ambientNr = 1;
    for(GLuint i = 0; i < this->textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);

        std::stringstream ss;
        std::string number;
        std::string name = this->textures[i].type;
        if(name == "texture_diffuse")
            ss << diffuseNr++;
        else if(name == "texture_specular")
            ss << specularNr++;
        else if(name == "texture_ambient")
            ss << ambientNr++;
        number = ss.str();

        glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
        glUniform1i(glGetUniformLocation(program, ("Material." + name + number).c_str()), i);
    }
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

Model::Model(std::string path)
{
    std::string modelName = findModelName(path);
    this->loadModel(path+"/"+modelName);
}

Model::~Model()
{
    for(unsigned i = 0; i < this->meshes.size(); i++)
    {
        delete this->meshes[i];
    }
    for(unsigned i = 0; i < this->textures_loaded.size(); i++)
    {
        glDeleteTextures(1, &this->textures_loaded[i].id);
    }
    this->meshes.clear();
    this->meshes.shrink_to_fit();
    this->textures_loaded.clear();
    this->textures_loaded.shrink_to_fit();
}

void Model::draw(GLuint program)
{
    for(GLuint i = 0; i < this->meshes.size(); i++)
        this->meshes[i]->draw(program);
}

std::string Model::findModelName(std::string folderPath)
{
    std::string model = "";
    DIR *dir;
    struct dirent *ent;
    dir = opendir(folderPath.c_str());
    if(!dir)
    {
        std::cout << "Cannot open folder: " << folderPath << std::endl;
        exit(1);
    }
    while ((ent = readdir (dir)) != NULL) {
        std::string filename = std::string(ent->d_name);
        std::string ext = filename.substr(filename.find_last_of(".") + 1);
        if(ext == "obj" ||
           ext == "3ds" ||
           ext == "gltf" ||
           ext == "fbx" ||
           ext == "dae" ||
           ext == "dxf" ||
           ext == "stl")
        {
            model = filename;
            break;
        }
    }
    closedir (dir);
    return model;
}

void Model::loadModel(std::string path)
{
    unsigned importFlags = aiProcess_Triangulate |
                           aiProcess_JoinIdenticalVertices |
                           aiProcess_SplitLargeMeshes |
                           aiProcess_PreTransformVertices |
                           aiProcess_ValidateDataStructure |
                           aiProcess_ImproveCacheLocality |
                           aiProcess_RemoveRedundantMaterials |
                           aiProcess_GenSmoothNormals |
                           aiProcess_FlipUVs |
                           aiProcess_FindInvalidData |
                           aiProcess_OptimizeMeshes;
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(path, importFlags);
    if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "Assimp ReadFile error: " << import.GetErrorString() << std::endl;
        exit(1);
    }
    this->directory = path.substr(0, path.find_last_of('/'));
    this->processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    for(unsigned i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        Mesh *processedMesh = this->processMesh(mesh, scene);
        this->meshes.push_back(processedMesh);
    }
    for(unsigned i = 0; i < node->mNumChildren; i++)
    {
        this->processNode(node->mChildren[i], scene);
    }
}

Mesh *Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;

    const aiVector3D aiZeroVector(0.0f, 0.0f, 0.0f);
    for(unsigned i = 0; i < mesh->mNumVertices; i++)
    {
        const aiVector3D* pPos = &(mesh->mVertices[i]);
		const aiVector3D* pNormal = &(mesh->mNormals[i]);
		const aiVector3D* pTexCoord = mesh->HasTextureCoords(0) ? &(mesh->mTextureCoords[0][i]) : &aiZeroVector;

		Vertex vertex = {glm::vec3(pPos->x, pPos->y, pPos->z),
				         glm::vec3(pNormal->x, pNormal->y, pNormal->z),
                         glm::vec2(pTexCoord->x, pTexCoord->y)};

        vertices.push_back(vertex);
    }
    // process indices
    for(unsigned i = 0; i < mesh->mNumFaces; i++)
    {
        const aiFace& face = mesh->mFaces[i];
        assert(face.mNumIndices == 3);
        indices.push_back(face.mIndices[0]);
        indices.push_back(face.mIndices[1]);
        indices.push_back(face.mIndices[2]);
    }
    // process material
    if(mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        std::vector<Texture> ambientMaps = this->loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_ambient");
        textures.insert(textures.end(), ambientMaps.begin(), ambientMaps.end());
    }
    Mesh* newMesh = new Mesh(vertices, indices, textures);
    return newMesh;
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
    std::vector<Texture> textures;
    for(GLuint i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        if(mat->GetTexture(type, i, &str) == AI_SUCCESS)
        {
            GLboolean skip = false;
            for(GLuint j = 0; j < textures_loaded.size(); j++)
            {
                if(textures_loaded[j].path == str)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true;
                    break;
                }
            }
            if(!skip)
            {
                Texture texture;
                texture.id = loadTexture(this->directory+"/"+str.C_Str());
                texture.type = typeName;
                texture.path = str;
                textures.push_back(texture);
                textures_loaded.push_back(texture);
            }
        }
        else
        {
            std::cout << "Error loading textures from material!" << std::endl;
            exit(1);
        }
    }
    return textures;
}

GLuint loadTextureArray(std::vector<std::string> path)
{
    std::vector<unsigned char *> data;
    int width, height;
    for(unsigned i = 0; i < path.size(); i++)
    {
        unsigned char *image = SOIL_load_image(path[i].c_str(), &width, &height, 0, SOIL_LOAD_RGB);
        if(!image)
        {
            std::cout << "Failed to load image: " << path[i] << std::endl;
            exit(1);
        }
        data.push_back(image);
    }
    GLuint texture;
    glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &texture);
    glTextureStorage3D(texture, 8, GL_RGB8, 256, 256, data.size());
    for(unsigned i = 0; i < data.size(); i++)
    {
        glTextureSubImage3D(texture, 0, 0, 0, i, 256, 256, 1, GL_RGB, GL_UNSIGNED_BYTE, data[i]);
    }
    for(unsigned i = 0; i < data.size(); i++)
        SOIL_free_image_data(data[i]);
    return texture;
}

GLuint loadTexture(std::string path)
{
    GLuint texture;
    int width, height;
    unsigned char *data = SOIL_load_image(path.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
    if(!data)
    {
        std::cout << "Failed to load image: " << path << std::endl;
        exit(1);
    }
    glCreateTextures(GL_TEXTURE_2D, 1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexStorage2D(GL_TEXTURE_2D, 4, GL_RGBA16, width, height);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateTextureMipmap(texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(data);
    return texture;
}

GLuint loadCubeMap(std::vector<std::string> path)
{
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
    unsigned char *data;
    int w, h;
    for(unsigned i = 0; i < path.size(); i++)
    {
        data = SOIL_load_image(path[i].c_str(), &w, &h, 0, SOIL_LOAD_RGB);
        if(data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            SOIL_free_image_data(data);
        }
        else
        {
            std::cout << "Failed to load image: " << path[i] << std::endl;
            exit(1);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    return tex;
}