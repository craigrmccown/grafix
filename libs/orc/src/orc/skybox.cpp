#include <glad/glad.h>
#include <memory>
#include "cubemap.hpp"
#include "mesh.hpp"
#include "skybox.hpp"

const std::vector<orc::Mesh::Vertex> vertices = {
    // Back face
    orc::Mesh::Vertex{.Coordinates = glm::vec3(-1.0f,  1.0f, -1.0f)},
    orc::Mesh::Vertex{.Coordinates = glm::vec3(-1.0f, -1.0f, -1.0f)},
    orc::Mesh::Vertex{.Coordinates = glm::vec3(1.0f, -1.0f, -1.0f)},
    orc::Mesh::Vertex{.Coordinates = glm::vec3(1.0f,  1.0f, -1.0f)},

    // Left face
    orc::Mesh::Vertex{.Coordinates = glm::vec3(-1.0f, -1.0f,  1.0f)},
    orc::Mesh::Vertex{.Coordinates = glm::vec3(-1.0f, -1.0f, -1.0f)},
    orc::Mesh::Vertex{.Coordinates = glm::vec3(-1.0f,  1.0f, -1.0f)},
    orc::Mesh::Vertex{.Coordinates = glm::vec3(-1.0f,  1.0f,  1.0f)},

    // Right face
    orc::Mesh::Vertex{.Coordinates = glm::vec3(1.0f, -1.0f, -1.0f)},
    orc::Mesh::Vertex{.Coordinates = glm::vec3(1.0f, -1.0f,  1.0f)},
    orc::Mesh::Vertex{.Coordinates = glm::vec3(1.0f,  1.0f,  1.0f)},
    orc::Mesh::Vertex{.Coordinates = glm::vec3(1.0f,  1.0f, -1.0f)},

    // Front face
    orc::Mesh::Vertex{.Coordinates = glm::vec3(-1.0f, -1.0f,  1.0f)},
    orc::Mesh::Vertex{.Coordinates = glm::vec3(-1.0f,  1.0f,  1.0f)},
    orc::Mesh::Vertex{.Coordinates = glm::vec3(1.0f,  1.0f,  1.0f)},
    orc::Mesh::Vertex{.Coordinates = glm::vec3(1.0f, -1.0f,  1.0f)},

    // Top face
    orc::Mesh::Vertex{.Coordinates = glm::vec3(-1.0f,  1.0f, -1.0f)},
    orc::Mesh::Vertex{.Coordinates = glm::vec3(1.0f,  1.0f, -1.0f)},
    orc::Mesh::Vertex{.Coordinates = glm::vec3(1.0f,  1.0f,  1.0f)},
    orc::Mesh::Vertex{.Coordinates = glm::vec3(-1.0f,  1.0f,  1.0f)},

    // Bottom face
    orc::Mesh::Vertex{.Coordinates = glm::vec3(-1.0f, -1.0f, -1.0f)},
    orc::Mesh::Vertex{.Coordinates = glm::vec3(-1.0f, -1.0f,  1.0f)},
    orc::Mesh::Vertex{.Coordinates = glm::vec3(1.0f, -1.0f,  1.0f)},
    orc::Mesh::Vertex{.Coordinates = glm::vec3(1.0f, -1.0f, -1.0f)},
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
    Skybox::Skybox(std::unique_ptr<CubemapRef> cubemap) : Mesh(vertices, indices, std::move(cubemap)) { }

    void Skybox::Draw()
    {
        // Change the depth testing function for this draw to ensure the skybox
        // is always behind other meshes
        glDepthFunc(GL_LEQUAL);
        Mesh::Draw();
        glDepthFunc(GL_LESS);
    }
}
