#pragma once

#include <map>
#include <string>
#include <glad/glad.h>
#include "texture.hpp"

namespace orc
{
    class Texture2D : public Texture
    {
        public:
        enum Type
        {
            BaseColor
        };

        Texture2D(Type type, const std::string &path);

        void Use() override;

        int64_t GetRenderSortKey() const override;

        private:
        GLenum format;
    };

    class Texture2DRef : public TextureRef
    {
        public:
        Texture2DRef(Texture2D::Type type, const std::string &path);

        Texture &Load() override;

        private:
        static std::map<std::string, std::unique_ptr<Texture2D>> cache;

        Texture2D::Type type;
        const std::string path;
    };
}
