#ifndef ASSETSTORE_HPP
#define ASSETSTORE_HPP

#include <memory>
#include <unordered_map>
#include <string>

#include <ngl/Obj.h>
#include <SDL2/SDL.h>

//The root folder for asset storage.
const std::string g_resourceLocation = "resources/";

class AssetStore
{
public:
    AssetStore() = default;
    ~AssetStore();
    AssetStore(const AssetStore &_store) = delete;

    //Getters for models and textures. These shouldn't really be needed,  as there
    //is a really cool function called 'drawAsset' in the scene class.
    ngl::Obj * getModel(const std::string &_id);
    GLuint getTexture(const std::string &_id);

    //Loads a mesh from _path, stores it with  _id.
    void loadMesh(const std::string &_id, const std::string &_path);
    //Loads a texture from _path, stores it with _id.
    void loadTexture(const std::string &_id, const std::string &_path);
private:
    //Internal method to help me load in images.
    GLuint SDLSurfaceToGLTexture( SDL_Surface * _surf );

    std::unordered_map< std::string, std::unique_ptr<ngl::Obj>> m_meshes;
    std::unordered_map< std::string, GLuint> m_textures;
};

#endif
