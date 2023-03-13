#include <memory>
#include <string>
#include <glad/glad.h>
#include "mesh.hpp"
#include "texture.hpp"

namespace orc
{
    Mesh::Mesh(std::vector<float> vertices, std::string texturePath)
        : vertices(vertices)
        , texture(std::make_unique<Texture>(texturePath))
    {
        // TODO: Implement OpenGL RAII library to prevent leaks on error
        glGenVertexArrays(1, &vaoId);
        glGenBuffers(1, &vboId);
        glGenBuffers(1, &eboId);

        // Bind buffer to vertex array and load vertices
        glBindVertexArray(vaoId);
        glBindBuffer(GL_ARRAY_BUFFER, vboId);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

        // Set vertex attributes to be interpreted by shader:
        // attribute location, number of elements, data type, whether to normalize,
        // stride to next element, offset
        //
        // Attribute 1, vertex coordinate: (x, y, z) @ location 0
        // Attribute 2, texture coordinate: (x, y) @ location 1
        // Attribute 3, normal vector: (x, y, z) @ location 2
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5*sizeof(float)));
        glEnableVertexAttribArray(2);
    }

    Mesh::~Mesh()
    {
        glDeleteVertexArrays(1, &vaoId);
        glDeleteBuffers(1, &vboId);
        glDeleteBuffers(1, &eboId);
    }

    void Mesh::Use()
    {
        texture->Use();
        glBindVertexArray(vaoId);
    }

    void Mesh::Draw()
    {
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    }
}
