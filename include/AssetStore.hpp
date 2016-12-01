#ifndef ASSETSTORE_HPP
#define ASSETSTORE_HPP

#include <memory>
#include <unordered_map>

#include <ngl/Obj.h>

class AssetStore
{
public:
    AssetStore() = default;
    ~AssetStore() = default;
    AssetStore(const AssetStore &_store) = delete;

    void loadMesh(const std::string &_id, const std::string &_path);
    void loadTexture(const std::string &_id, const std::string &_path);
private:
    std::unordered_map< std::string, std::unique_ptr<ngl::Obj>> m_meshes;
    std::unordered_map< std::string, std::unique_ptr<GLuint>> m_textures;
};

#endif
