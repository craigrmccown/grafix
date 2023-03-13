#pragma once

#include <string>

namespace orc
{
    class Texture
    {
        public:
        Texture(std::string path);

        ~Texture();

        // Disable copy construction and assignment to avoid destruction of managed 
        // OpenGL objects
        Texture(const Texture &other) = delete;
        void operator=(const Texture &other) = delete;

        void Use();

        unsigned int GetId();

        private:
        unsigned int id;
    };
}
