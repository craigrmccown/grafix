#pragma once

#include <memory>
#include <string>
#include <vector>
#include "texture.hpp"

namespace orc
{
    class Mesh 
    {
        public:
        Mesh(std::vector<float> vertices, std::string texturePath);

        ~Mesh();

        // Disable copy construction and assignment to avoid destruction of managed 
        // OpenGL objects
        Mesh(const Mesh &other) = delete;
        void operator=(const Mesh &other) = delete;

        void Use();

        void Draw();

        private:
        std::unique_ptr<Texture> texture;

        // Vertex Array Object
        unsigned int vaoId;

        // Vertex Buffer Object
        unsigned int vboId;

        // Element Buffer Object (not currently used)
        unsigned int eboId;

        std::vector<float> vertices;
    };
}
