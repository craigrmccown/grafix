#include <filesystem>
#include <memory>
#include <glm/glm.hpp>
#include "cube.hpp"
#include "mesh.hpp"

const std::vector<orc::Mesh::Vertex> vertices = {
    // Back face
    orc::Mesh::Vertex {
        .Coordinates = glm::vec3(-0.5f, -0.5f, -0.5f),
        .TextureCoords = glm::vec2(0.0f, 0.0f),
        .Normal = glm::vec3(0.0f, 0.0f, -1.0f),
    },
    orc::Mesh::Vertex {
        .Coordinates = glm::vec3(0.5f, -0.5f, -0.5f),
        .TextureCoords = glm::vec2(1.0f, 0.0f),
        .Normal = glm::vec3(0.0f, 0.0f, -1.0f),
    },
    orc::Mesh::Vertex {
        .Coordinates = glm::vec3(0.5f, 0.5f, -0.5f),
        .TextureCoords = glm::vec2(1.0f, 1.0f),
        .Normal = glm::vec3(0.0f, 0.0f, -1.0f),
    },
    orc::Mesh::Vertex {
        .Coordinates = glm::vec3(-0.5f, 0.5f, -0.5f),
        .TextureCoords = glm::vec2(0.0f, 1.0f),
        .Normal = glm::vec3(0.0f, 0.0f, -1.0f),
    },

    // Front face
    orc::Mesh::Vertex {
        .Coordinates = glm::vec3(-0.5f, -0.5f, 0.5f),
        .TextureCoords = glm::vec2(0.0f, 0.0f),
        .Normal = glm::vec3(0.0f, 0.0f, 1.0f),
    },
    orc::Mesh::Vertex {
        .Coordinates = glm::vec3(0.5f, -0.5f, 0.5f),
        .TextureCoords = glm::vec2(1.0f, 0.0f),
        .Normal = glm::vec3(0.0f, 0.0f, 1.0f),
    },
    orc::Mesh::Vertex {
        .Coordinates = glm::vec3(0.5f, 0.5f, 0.5f),
        .TextureCoords = glm::vec2(1.0f, 1.0f),
        .Normal = glm::vec3(0.0f, 0.0f, 1.0f),
    },
    orc::Mesh::Vertex {
        .Coordinates = glm::vec3(-0.5f, 0.5f, 0.5f),
        .TextureCoords = glm::vec2(0.0f, 1.0f),
        .Normal = glm::vec3(0.0f, 0.0f, 1.0f),
    },

    // Left face
    orc::Mesh::Vertex {
        .Coordinates = glm::vec3(-0.5f, 0.5f, 0.5f),
        .TextureCoords = glm::vec2(1.0f, 0.0f),
        .Normal = glm::vec3(-1.0f, 0.0f, 0.0f),
    },
    orc::Mesh::Vertex {
        .Coordinates = glm::vec3(-0.5f, 0.5f, -0.5f),
        .TextureCoords = glm::vec2(1.0f, 1.0f),
        .Normal = glm::vec3(-1.0f, 0.0f, 0.0f),
    },
    orc::Mesh::Vertex {
        .Coordinates = glm::vec3(-0.5f, -0.5f, -0.5f),
        .TextureCoords = glm::vec2(0.0f, 1.0f),
        .Normal = glm::vec3(-1.0f, 0.0f, 0.0f),
    },
    orc::Mesh::Vertex {
        .Coordinates = glm::vec3(-0.5f, -0.5f, 0.5f),
        .TextureCoords = glm::vec2(0.0f, 0.0f),
        .Normal = glm::vec3(-1.0f, 0.0f, 0.0f),
    },

    // Right face
    orc::Mesh::Vertex {
        .Coordinates = glm::vec3(0.5f, 0.5f, 0.5f),
        .TextureCoords = glm::vec2(1.0f, 0.0f),
        .Normal = glm::vec3(1.0f, 0.0f, 0.0f),
    },
    orc::Mesh::Vertex {
        .Coordinates = glm::vec3(0.5f, 0.5f, -0.5f),
        .TextureCoords = glm::vec2(1.0f, 1.0f),
        .Normal = glm::vec3(1.0f, 0.0f, 0.0f),
    },
    orc::Mesh::Vertex {
        .Coordinates = glm::vec3(0.5f, -0.5f, -0.5f),
        .TextureCoords = glm::vec2(0.0f, 1.0f),
        .Normal = glm::vec3(1.0f, 0.0f, 0.0f),
    },
    orc::Mesh::Vertex {
        .Coordinates = glm::vec3(0.5f, -0.5f, 0.5f),
        .TextureCoords = glm::vec2(0.0f, 0.0f),
        .Normal = glm::vec3(1.0f, 0.0f, 0.0f),
    },

    // Bottom face
    orc::Mesh::Vertex {
        .Coordinates = glm::vec3(-0.5f, -0.5f, -0.5f),
        .TextureCoords = glm::vec2(0.0f, 1.0f),
        .Normal = glm::vec3(0.0f, -1.0f, 0.0f),
    },
    orc::Mesh::Vertex {
        .Coordinates = glm::vec3(0.5f, -0.5f, -0.5f),
        .TextureCoords = glm::vec2(1.0f, 1.0f),
        .Normal = glm::vec3(0.0f, -1.0f, 0.0f),
    },
    orc::Mesh::Vertex {
        .Coordinates = glm::vec3(0.5f, -0.5f, 0.5f),
        .TextureCoords = glm::vec2(1.0f, 0.0f),
        .Normal = glm::vec3(0.0f, -1.0f, 0.0f),
    },
    orc::Mesh::Vertex {
        .Coordinates = glm::vec3(-0.5f, -0.5f, 0.5f),
        .TextureCoords = glm::vec2(0.0f, 0.0f),
        .Normal = glm::vec3(0.0f, -1.0f, 0.0f),
    },

    // Top face
    orc::Mesh::Vertex {
        .Coordinates = glm::vec3(-0.5f,  0.5f, -0.5f),
        .TextureCoords = glm::vec2(0.0f, 1.0f),
        .Normal = glm::vec3(0.0f, 1.0f, 0.0f),
    },
    orc::Mesh::Vertex {
        .Coordinates = glm::vec3(0.5f,  0.5f, -0.5f),
        .TextureCoords = glm::vec2(1.0f, 1.0f),
        .Normal = glm::vec3(0.0f, 1.0f, 0.0f),
    },
    orc::Mesh::Vertex {
        .Coordinates = glm::vec3(0.5f,  0.5f, 0.5f),
        .TextureCoords = glm::vec2(1.0f, 0.0f),
        .Normal = glm::vec3(0.0f, 1.0f, 0.0f),
    },
    orc::Mesh::Vertex {
        .Coordinates = glm::vec3(-0.5f,  0.5f, 0.5f),
        .TextureCoords = glm::vec2(0.0f, 0.0f),
        .Normal = glm::vec3(0.0f, 1.0f, 0.0f),
    },
};

const std::vector<unsigned int> indices = {
    0, 1, 2, 2, 3, 0,
    4, 5, 6, 6, 7, 4,
    8, 9, 10, 10, 11, 8,
    12, 13, 14, 14, 15, 12,
    16, 17, 18, 18, 19, 16,
    20, 21, 22, 22, 23, 20,
};

namespace orc
{
    std::unique_ptr<Mesh> BuildCubeMesh(std::string dataDir)
    {
        return std::make_unique<Mesh>(
            vertices,
            indices,
            std::filesystem::path(dataDir) / std::filesystem::path("textures") / std::filesystem::path("crate.png")
        );
    }
}
