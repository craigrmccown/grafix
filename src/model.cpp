#include <memory>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "model.hpp"
#include "texture.hpp"

Model::Model(std::vector<float> vertices, std::string texturePath)
    : texturePath(texturePath)
    , vertices(vertices)
{
    texture = std::make_unique<Texture>();
    glGenVertexArrays(1, &vaoId);
    glGenBuffers(1, &vboId);
    glGenBuffers(1, &eboId);
}

Model::~Model()
{
    glDeleteVertexArrays(1, &vaoId);
    glDeleteBuffers(1, &vboId);
    glDeleteBuffers(1, &eboId);
}

void Model::load()
{
    texture->load(texturePath);

    glBindVertexArray(vaoId);

    // Load vertices
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    // Set vertex attributes to be interpreted by shader:
    // attribute location, number of elements, data type, whether to normalize,
    // stride to next element, offset
    //
    // Attribute 1, vertex coordinate: (x, y, z) @ location 0
    // Attribute 2, texture coordinate: (x, y) @ location 1
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
}

void Model::draw()
{
    texture->use();
    glBindVertexArray(vaoId);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}
