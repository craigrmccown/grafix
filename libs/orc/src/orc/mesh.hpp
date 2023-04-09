#pragma once

#include <memory>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "texture.hpp"
#include "texture_ref.hpp"

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

        Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices, TextureRef texture);

        ~Mesh();

        // Disable copy construction and assignment to avoid destruction of managed 
        // OpenGL objects
        Mesh(const Mesh &other) = delete;
        void operator=(const Mesh &other) = delete;

        // Checks whether the underlying base texture has an alpha channel. In
        // order to perform this check, the texture is fully loaded if it is not
        // already.
        bool IsTransparent();

        void Use();

        void Draw();

        private:
        // Vertex Array Object
        unsigned int vaoId;

        // Vertex Buffer Object
        unsigned int vboId;

        // Element Buffer Object
        unsigned int eboId;

        const std::vector<Vertex> vertices;
        const std::vector<unsigned int> indices;

        // TODO: Support multiple textures
        TextureRef texture;
    };
}
