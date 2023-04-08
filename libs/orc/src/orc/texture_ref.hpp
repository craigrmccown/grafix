#pragma once

#include "texture.hpp"

namespace orc
{
    class TextureRef
    {
        public:
        TextureRef(Texture::Type type, const std::string &path);

        Texture &Load();

        private:
        Texture::Type type;
        const std::string path;
    };
}
