#include <cstdint>
#include <memory>
#include <string>
#include <glad/glad.h>
#include "mesh.hpp"
#include "texture.hpp"

namespace orc
{
    Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices, std::unique_ptr<TextureRef> texture)
        : vertices(vertices)
        , indices(indices)
        , texture(std::move(texture))
    {
        // TODO: Implement OpenGL RAII library to prevent leaks on error
        glGenVertexArrays(1, &vaoId);
        glGenBuffers(1, &vboId);
        glGenBuffers(1, &eboId);

        // Bind buffer to vertex array and load vertices
        glBindVertexArray(vaoId);
        glBindBuffer(GL_ARRAY_BUFFER, vboId);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        // Do the same thing for indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // TODO: Configurable vertex types - not all shaders need all of these attributes
        // Set vertex attributes to be interpreted by shader:
        // attribute location, number of elements, data type, whether to normalize,
        // stride to next element, offset. The Vertex struct is used to provide
        // semantic meaning to each attribute.
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Coordinates));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TextureCoords));
        glEnableVertexAttribArray(2);
    }

    Mesh::~Mesh()
    {
        glDeleteVertexArrays(1, &vaoId);
        glDeleteBuffers(1, &vboId);
        glDeleteBuffers(1, &eboId);
    }

    Texture &Mesh::GetTexture() const
    {
        return texture->Load();
    }

    void Mesh::Use()
    {
        // TODO: Default texture if none provided
        texture->Load().Use();
        glBindVertexArray(vaoId);
    }

    void Mesh::Draw()
    {
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    }
}
