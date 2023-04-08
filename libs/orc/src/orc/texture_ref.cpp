#include "texture.hpp"
#include "texture_manager.hpp"
#include "texture_ref.hpp"

namespace orc
{
    TextureRef::TextureRef(Texture::Type type, const std::string &path)
        : type(type)
        , path(path)
        {}
    
    Texture &TextureRef::Load()
    {
        return TextureManager::Get().LoadTexture(type, path);
    }
}
