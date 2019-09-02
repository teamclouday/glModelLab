#include "manager.hpp"

GlobalManager::GlobalManager()
{
    // do nothing here
}

GlobalManager::~GlobalManager()
{
    if(!this->myWindow)
        SDL_DestroyWindow(this->myWindow);
    if(!this->myContext)
        SDL_GL_DeleteContext(this->myContext);
    if(!this->myCamera)
        delete this->myCamera;
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
    }
}