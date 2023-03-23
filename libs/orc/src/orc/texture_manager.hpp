#pragma once

#include <map>
#include <memory>
#include "texture.hpp"

namespace orc
{
    // TODO: Limit number of textures loaded into vRAM
    class TextureManager
    {
        public:
        Texture &LoadTexture(std::string path);

        private:
        std::map<std::string, std::unique_ptr<Texture>> textures;
    };
}
