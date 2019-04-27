#include "model.hpp"

Model::Model(std::string path)
{
    this->loadModel(path);
}

Model::~Model()
{
    //std::vector<Mesh>().swap(this->meshes);
    //std::vector<Texture>().swap(this->textures_loaded);
    for(int i = 0; i < this->meshes.size(); i++)
    {
        glDeleteBuffers(1, &this->meshes[i]->VBO);
        glDeleteBuffers(1, &this->meshes[i]->EBO);
        glDeleteVertexArrays(1, &this->meshes[i]->VAO);
        delete this->meshes[i];

    }
    for(int i = 0; i < this->textures_loaded.size(); i++)
    {
        glDeleteTextures(1, &this->textures_loaded[i].id);
    }
    this->meshes.clear();
    this->meshes.shrink_to_fit();
    this->textures_loaded.clear();
    this->textures_loaded.shrink_to_fit();
}

void Model::draw(Shader *shader)
{
    for(GLuint i = 0; i < this->meshes.size(); i++)
        this->meshes[i]->draw(shader);
}

bool Model::exists()
{
    return this->meshes.size() != 0;
}

void Model::loadModel(std::string path)
{
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate |
                                                 aiProcess_CalcTangentSpace |
                                                 aiProcess_JoinIdenticalVertices |
                                                 aiProcess_MakeLeftHanded |
                                                 aiProcess_GenSmoothNormals |
                                                 aiProcess_RemoveComponent |
                                                 aiProcess_SplitLargeMeshes |
                                                 aiProcess_PreTransformVertices |
                                                 aiProcess_ValidateDataStructure |
                                                 aiProcess_ImproveCacheLocality |
                                                 // aiProcess_RemoveRedundantMaterials |
                                                 aiProcess_FixInfacingNormals |
                                                 aiProcess_FindInvalidData |
                                                 aiProcess_TransformUVCoords |
                                                 aiProcess_FindInstances |
                                                 aiProcess_FlipUVs |
                                                 aiProcess_OptimizeMeshes
                                                 );
    if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        printf("Assimp ReadFile error: %s\n", import.GetErrorString());
        return;
    }
    this->directory = path.substr(0, path.find_last_of('/'));
    this->processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    for(GLuint i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        this->meshes.push_back(this->processMesh(mesh, scene));
    }
    for(GLuint i = 0; i < node->mNumChildren; i++)
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
    for(GLuint i = 0; i < mesh->mNumVertices; i++)
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
    for(GLuint i = 0; i < mesh->mNumFaces; i++)
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
    }

    glFlush();

    return new Mesh(vertices, indices, textures);
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
                texture.id = loadTextureFromFile(str.C_Str(), this->directory, false);
                texture.type = typeName;
                texture.path = str;
                textures.push_back(texture);
                textures_loaded.push_back(texture);
            }
        }
        else
        {
            printf("Error loading textures from material!\n");
        }
    }
    return textures;
}

GLuint loadTextureFromFile(const std::string filename, const std::string directory, GLboolean alpha)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    int width, height;
    unsigned char *image = SOIL_load_image(((directory + '/' + filename).c_str()), &width, &height, 0, alpha ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB);
    if(!image)
    {
        printf("Cannot load texture: %s!\n", filename.c_str());
        return 0;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, alpha ? GL_RGBA : GL_RGB, width, height, 0, alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    // configure texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, alpha ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, alpha ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    // end of configure texture
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
}