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
        static TextureManager &Get();

        Texture &LoadTexture(Texture::Type type, std::string path);

        // Delete copy constructor and copy assignment since we use this as a
        // singleton
        TextureManager(TextureManager const &other) = delete;
        void operator=(TextureManager const &other) = delete;

        private:
        // Make constructor private since we use this as a singleton
        TextureManager();

        std::map<std::string, std::unique_ptr<Texture>> textures;
    };
}
