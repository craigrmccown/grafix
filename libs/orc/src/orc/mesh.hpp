#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "texture.hpp"

namespace orc
{
    class Mesh 
    {
        public:
        struct Vertex
        {
            glm::vec3 Coordinates;
            glm::vec3 Normal;
            glm::vec2 TextureCoords;
        };

        Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices, std::unique_ptr<TextureRef> texture);

        ~Mesh();

        // Disable copy construction and assignment to avoid destruction of managed 
        // OpenGL objects
        Mesh(const Mesh &other) = delete;
        void operator=(const Mesh &other) = delete;

        Texture &GetTexture() const;

        void Use();

        virtual void Draw();

        private:
        // Vertex Array Object
        unsigned int vaoId;

        // Vertex Buffer Object
        unsigned int vboId;

        // Element Buffer Object
        unsigned int eboId;

        // The number of indices passed to glDrawElements
        int numIndices;

        // TODO: Support multiple textures (material system)
        std::unique_ptr<TextureRef> texture;
    };
}
