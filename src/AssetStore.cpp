#include <SDL2/SDL.h>

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
    return;
}
