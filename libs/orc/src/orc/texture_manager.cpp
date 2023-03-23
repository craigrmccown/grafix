#include <memory>
#include "texture.hpp"
#include "texture_manager.hpp"

namespace orc
{
    Texture &TextureManager::LoadTexture(std::string path)
    {
        if (!textures.count(path))
        {
            textures[path] = std::make_unique<Texture>(path);
        }
        
        return *textures[path];
    }
}
