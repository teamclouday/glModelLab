#include "model.hpp"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, Texture tex)
{
    this->vertices = vertices;
    this->indices = indices;
    this->myTexture = tex;
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
    if(this->myTexture.texCount > 0)
    {
        glDeleteTextures(1, &this->myTexture.diffuse_id);
        if(this->myTexture.texCount > 1)
            glDeleteTextures(1, &this->myTexture.emission_id);
    }
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

    // GLuint diffuseNr = 1;
    // GLuint specularNr = 1;
    // for(GLuint i = 0; i < this->textures.size(); i++)
    // {
    //     glActiveTexture(GL_TEXTURE0 + i);

    //     std::stringstream ss;
    //     std::string number;
    //     std::string name = this->textures[i].type;
    //     if(name == "texture_diffuse")
    //         ss << diffuseNr++;
    //     else if(name == "texture_specular")
    //         ss << specularNr++;
    //     number = ss.str();

    //     glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
    //     glUniform1i(glGetUniformLocation(program, ("Material." + name + number).c_str()), i);
    // }
    // if(!this->textures.size())
    //     glUniform1f(glGetUniformLocation(program, "material_exists"), 0.0f);
    // else
    // {
    //     glUniform1f(glGetUniformLocation(program, "material_alpha"), this->textures[0].alpha);
    //     glUniform1f(glGetUniformLocation(program, "material_exists"), 1.0f);
    // }
    glUniform4fv(glGetUniformLocation(program, "material.diffuse"), 1, &this->myTexture.diffuse[0]);
    glUniform4fv(glGetUniformLocation(program, "material.emissive"), 1, &this->myTexture.emissive[0]);
    glUniform1fv(glGetUniformLocation(program, "material.shininess"), 1, &this->myTexture.shininess);
    glUniform1i(glGetUniformLocation(program, "material.texCount"), this->myTexture.texCount);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->myTexture.diffuse_id);
    glUniform1i(glGetUniformLocation(program, "material.tex_diff"), 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->myTexture.emission_id);
    glUniform1i(glGetUniformLocation(program, "material.tex_emi"), 1);
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
    // for(unsigned i = 0; i < this->textures_loaded.size(); i++)
    // {
    //     glDeleteTextures(1, &this->textures_loaded[i].id);
    // }
    this->meshes.clear();
    this->meshes.shrink_to_fit();
    // this->textures_loaded.clear();
    // this->textures_loaded.shrink_to_fit();
}

void Model::draw(GLuint program)
{
    for(GLuint i = 0; i < this->meshes.size(); i++)
        this->meshes[i]->draw(program);
}

std::string Model::findModelName(std::string folderPath)
{
    std::string model = "";
#ifdef __unix__
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
#endif

#ifdef _WIN32
    WIN32_FIND_DATA fd;
    HANDLE hFind;
	hFind = FindFirstFile(LPCSTR((folderPath + "/*").c_str()), &fd);
    if(hFind != INVALID_HANDLE_VALUE)
    {
		do
		{
			std::string filename = std::string(fd.cFileName);
			// std::string filename((const char*)&name[0], sizeof(wchar_t)/sizeof(char)*name.size());
			std::string ext = filename.substr(filename.find_last_of(".") + 1);
			if (ext == "obj" ||
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
		} while (FindNextFile(hFind, &fd));
		FindClose(hFind);
    }
#endif
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
    // const aiScene* scene = import.ReadFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
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
    Texture texture;

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
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    aiString texPath;
    texture.diffuse_id = 0;
    texture.emission_id = 0;
    texture.texCount = 0;
    if(AI_SUCCESS == material->GetTexture(aiTextureType_DIFFUSE, 0, &texPath))
    {
        texture.diffuse_id = loadTexture(this->directory+"/"+texPath.C_Str());
        texture.texCount += 1;
        if(AI_SUCCESS == material->GetTexture(aiTextureType_EMISSIVE, 0, &texPath))
        {
            texture.emission_id = loadTexture(this->directory+"/"+texPath.C_Str());
            texture.texCount += 1;
        }
    }
    aiColor4D color;
    // set default diffuse color if not exists
    texture.diffuse = glm::vec4(0.4f, 0.5f, 0.6f, 1.0f);
    if(AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &color))
    {
        texture.diffuse = glm::vec4(color.r, color.g, color.b, color.a);
    }
    if(AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_EMISSIVE, &color))
    {
        texture.emissive = glm::vec4(color.r, color.g, color.b, color.a);
    }
    float data = 0.0f;
    texture.shininess = 20.0f; // set default shininess value
    if(AI_SUCCESS == aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &data))
    {
        if(data >= 1.0f)
            texture.shininess = data;
    }
    // texture.alpha = 1.0f;
    // if(AI_SUCCESS == aiGetMaterialFloatArray(material, AI_MATKEY_OPACITY, &data, &max))
    //     texture.alpha = data;
    // if(mesh->mMaterialIndex >= 0)
    // {
    //     aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    //     std::vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    //     textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    //     std::vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    //     textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // }
    Mesh* newMesh = new Mesh(vertices, indices, texture);
    return newMesh;
}

// std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
// {
//     std::vector<Texture> textures;
//     for(GLuint i = 0; i < mat->GetTextureCount(type); i++)
//     {
//         aiString str;
//         if(mat->GetTexture(type, i, &str) == AI_SUCCESS)
//         {
//             bool skip = false;
//             for(GLuint j = 0; j < textures_loaded.size(); j++)
//             {
//                 if(textures_loaded[j].path == str)
//                 {
//                     textures.push_back(textures_loaded[j]);
//                     skip = true;
//                     break;
//                 }
//             }
//             if(!skip)
//             {
//                 Texture texture;
//                 texture.id = loadTexture(this->directory+"/"+str.C_Str());
//                 texture.type = typeName;
//                 texture.path = str;
//                 texture.alpha = 1.0f;
//                 float alpha = 0.0f;
//                 if(aiGetMaterialFloat(mat, AI_MATKEY_OPACITY, &alpha) == AI_SUCCESS)
//                     texture.alpha = alpha;
//                 else if(aiGetMaterialFloat(mat, AI_MATKEY_TRANSPARENCYFACTOR, &alpha) == AI_SUCCESS)
//                     texture.alpha = alpha;
//                 textures.push_back(texture);
//                 textures_loaded.push_back(texture);
//             }
//         }
//         else
//         {
//             std::cout << "Error loading textures from material!" << std::endl;
//             exit(1);
//         }
//     }
//     return textures;
// }

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
    int width, height, channels;
    unsigned char *data = SOIL_load_image(path.c_str(), &width, &height, &channels, SOIL_LOAD_AUTO);
    if(!data)
    {
        std::cout << "Failed to load image: " << path << std::endl;
        std::cout << SOIL_last_result() << std::endl;
        exit(1);
    }
    GLenum format = GL_RGBA;
    switch(channels)
    {
        case 1:
            format = GL_RED;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            break;
    }
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // glTexStorage2D(GL_TEXTURE_2D, 4, format, width, height);
    // glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateTextureMipmap(texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
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