#pragma once

#include <memory>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "texture.hpp"
#include "texture_manager.hpp"

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

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::string texturePath);

        ~Mesh();

        // Disable copy construction and assignment to avoid destruction of managed 
        // OpenGL objects
        Mesh(const Mesh &other) = delete;
        void operator=(const Mesh &other) = delete;

        void Use(TextureManager &TextureManager);

        void Draw();

        private:
        // Vertex Array Object
        unsigned int vaoId;

        // Vertex Buffer Object
        unsigned int vboId;

        // Element Buffer Object (not currently used)
        unsigned int eboId;

        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        // TODO: Support multiple textures
        std::string texturePath;
    };
}