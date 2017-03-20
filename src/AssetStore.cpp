#include <algorithm>

#include <SDL_image.h>

#include "AssetStore.hpp"

AssetStore::~AssetStore()
{
    for(auto &tex : m_textures)
        glDeleteTextures( 1, &tex.second );
}

ngl::Obj * AssetStore::getModel(const std::string &_id)
{
    auto it = m_meshes.find( _id );
    if(it != m_meshes.end())
        return it->second.get();
    return nullptr;
}

GLuint AssetStore::getTexture(const std::string &_id)
{
    auto it = m_textures.find( _id );
    if(it != m_textures.end())
        return it->second;
    return 0;
}

void AssetStore::loadMesh(const std::string &_id, const std::string &_path)
{
    std::cout << "Loading mesh from " << _path << '\n';
    ngl::Obj * mesh = new ngl::Obj(g_resourceLocation + "geometry/" + _path);
    if(mesh == nullptr)
    {
        std::cerr << "Uh oh! Mesh from " << g_resourceLocation + "geometry/" + _path <<  " could not be loaded. Aborting...";
        return;
    }
    mesh->createVAO();

    //m_meshes now responsible for deleting mesh
    m_meshes.insert(std::make_pair(_id, std::unique_ptr<ngl::Obj>(mesh)));
}

void AssetStore::loadTexture(const std::string &_id, const std::string &_path)
{
    std::cout << "Loading texture from " << _path << '\n';
    SDL_Surface * surf = IMG_Load((g_resourceLocation + "textures/" + _path).c_str());
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

    int mode = GL_RGB;

    if(_surf->format->BytesPerPixel == 4) {
        mode = GL_RGBA;
    }

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, mode, _surf->w, _surf->h, 0, mode, GL_UNSIGNED_BYTE, _surf->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return textureID;
}
