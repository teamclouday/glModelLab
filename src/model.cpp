#include "model.hpp"

Model::Model()
{
    loadList();
    modelIdx = 0;
}

void Model::loadList()
{
    DIR *d;
    struct dirent *dir;
    d = opendir("./Models");
    while((dir = readdir(d)) != NULL)
    {
        model_path.push_back(dir->d_name);
    }
    closedir(d);
}

void Model::displayCheckList()
{
    for(int i = 2; i < model_path.size(); i++)
    {
        ImGui::RadioButton(model_path[i].c_str(), &this->modelIdx, i);
    }
    if(model_path.size() <= 2)
    {
        ImGui::Text("No models found under folder Models");
    }
}