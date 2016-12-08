#include <SDL_image.h>

#include "AssetStore.hpp"

void AssetStore::loadMesh(const std::string &_id, const std::string &_path)
{
    ngl::Obj * mesh = new ngl::Obj( _path );
    if(mesh == nullptr)
    {
        std::cerr << "Uh oh! Mesh from " << _path <<  " could not be loaded. Aborting...";
        return;
    }
    mesh->createVAO();

    //m_meshes now responsible for deleting mesh
    m_meshes.insert( std::make_pair( _id, std::unique_ptr<ngl::Obj>(mesh) ) );
}

void AssetStore::loadTexture(const std::string &_id, const std::string &_path)
{
    SDL_Surface * surf = IMG_Load(_path.c_str());
    GLuint tex;
    if(surf != nullptr)
        tex = SDLSurfaceToGLTexture(surf);
    else
    {
        std::cerr << "Warning! Null surface. " << SDL_GetError() << '\n';
        return;
    }

    SDL_FreeSurface(surf);

    m_textures.insert( std::make_pair( _id, tex) );
}

GLuint AssetStore::SDLSurfaceToGLTexture(SDL_Surface *_surf)
{
    GLuint textureID;

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _surf->w, _surf->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, _surf->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return textureID;
}
