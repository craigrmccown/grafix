#pragma once

#include <string>
#include <glad/glad.h>

namespace orc
{
    class Texture
    {
        public:
        enum Type
        {
            Diffuse
        };

        Texture(Type type, std::string path);

        ~Texture();

        // Disable copy construction and assignment to avoid destruction of managed 
        // OpenGL objects
        Texture(const Texture &other) = delete;
        void operator=(const Texture &other) = delete;

        void Use();

        unsigned int GetId() const;

        bool HasAlphaChannel() const;

        private:
        unsigned int id;
        GLenum format;
    };
}
