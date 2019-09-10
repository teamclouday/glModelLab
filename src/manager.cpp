#include "manager.hpp"

GlobalManager::GlobalManager(ProgramInfo *info)
{
    this->info = info;
    this->fpsLimit = true;
    this->update();
}

GlobalManager::~GlobalManager()
{
    if(!this->myWindow)
        SDL_DestroyWindow(this->myWindow);
    if(!this->myContext)
        SDL_GL_DeleteContext(this->myContext);
    if(!this->myCamera)
        delete this->myCamera;
    if(!this->myRenderer)
        delete this->myRenderer;
    if(!this->info)
        delete this->info;
}

// return true if previous value is overrided
bool GlobalManager::store(GlobalVariables var, void* pointer)
{
    bool overrided = false;
    switch(var)
    {
        case GLOB_WINDOW:
        {
            if(this->myWindow)
            {
                overrided = true;
                this->free(var);
            }
            this->myWindow = (SDL_Window*)pointer;
            break;
        }
        case GLOB_CONTEXT:
        {
            if(this->myContext)
            {
                overrided = true;
                this->free(var);
            }
            this->myContext = (SDL_GLContext)pointer;
            break;
        }
        case GLOB_CAMERA:
        {
            if(this->myCamera)
            {
                overrided = true;
                this->free(var);
            }
            this->myCamera = (Camera*)pointer;
            break;
        }
        case GLOB_RENDERER:
        {
            if(this->myRenderer)
            {
                overrided = true;
                this->free(var);
            }
            this->myRenderer = (Renderer*)pointer;
            break;
        }
    }
    return overrided;
}

// free var
void GlobalManager::free(GlobalVariables var)
{
    switch(var)
    {
        case GLOB_WINDOW:
        {
            if(!this->myWindow)
                SDL_DestroyWindow(this->myWindow);
            this->myWindow = nullptr;
            break;
        }
        case GLOB_CONTEXT:
        {
            if(!this->myContext)
                SDL_GL_DeleteContext(this->myContext);
            this->myContext = nullptr;
            break;
        }
        case GLOB_CAMERA:
        {
            if(!this->myCamera)
                delete this->myCamera;
            this->myCamera = nullptr;
            break;
        }
        case GLOB_RENDERER:
        {
            if(!this->myRenderer)
                delete this->myRenderer;
            this->myRenderer = nullptr;
            break;
        }
    }
}

void GlobalManager::update()
{
    this->models.clear();
    this->models.shrink_to_fit();
    this->shaders.clear();
    this->shaders.shrink_to_fit();
    this->load_models();
    this->load_shaders();
}

void GlobalManager::load_models()
{
#ifdef __unix__
    DIR *d;
    struct dirent *dir;
    d = opendir(std::string(MY_ROOT_DIR) + "/models");
    if(!d)
    {
        std::cout << "\"models\" folder not found" << std::endl;
        exit(1);
    }
    while((dir = readdir(d)) != NULL)
    {
        if(dir->d_name[0] != '.')
            this->models.push_back(dir->d_name);
    }
    closedir(d);
#endif

#ifdef _WIN32
    WIN32_FIND_DATA fd;
    HANDLE hFind;
    hFind = FindFirstFile(LPCSTR((std::string(MY_ROOT_DIR) + "/models/*").c_str()), &fd);
    if(hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            std::string filename = std::string(fd.cFileName);
            if(filename[0] != '.')
                this->models.push_back(filename);
        } while(FindNextFile(hFind, &fd));
        FindClose(hFind);
    }
    else
    {
        std::cout << "\"models\" folder not found" << std::endl;
        exit(1);
    }
#endif
}

void GlobalManager::load_shaders()
{
#ifdef __unix__
    DIR *d;
    struct dirent *dir;
    d = opendir(std::string(MY_ROOT_DIR) + "/shaders");
    if(!d)
    {
        std::cout << "\"shaders\" folder not found" << std::endl;
        exit(1);
    }
    while((dir = readdir(d)) != NULL)
    {
        if(dir->d_name[0] != '.')
            this->shaders.push_back(dir->d_name);
    }
    closedir(d);
#endif

#ifdef _WIN32
    WIN32_FIND_DATA fd;
    HANDLE hFind;
    hFind = FindFirstFile(LPCSTR((std::string(MY_ROOT_DIR) + "/shaders/*").c_str()), &fd);
    if(hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            std::string filename = std::string(fd.cFileName);
            if(filename[0] != '.')
                this->shaders.push_back(filename);
        } while(FindNextFile(hFind, &fd));
        FindClose(hFind);
    }
    else
    {
        std::cout << "\"shaders\" folder not found" << std::endl;
        exit(1);
    }
#endif
}