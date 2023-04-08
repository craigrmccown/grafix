#include <memory>
#include "texture.hpp"
#include "texture_manager.hpp"

namespace orc
{
    TextureManager::TextureManager() {}

    TextureManager &TextureManager::Get()
    {
        // Lazily initialized, should be thread safe.
        static TextureManager tm;
        return tm;
    }

    Texture &TextureManager::LoadTexture(Texture::Type type, std::string path)
    {
        if (!textures.count(path))
        {
            textures[path] = std::make_unique<Texture>(type, path);
        }
        
        return *textures[path];
    }
}
