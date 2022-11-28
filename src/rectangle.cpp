#include <memory>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "rectangle.hpp"
#include "texture.hpp"

float vertices[] = {
    // positions            // texture coords
    -0.5f, 0.5f, 0.0f,      0.0f, 1.0f,     // top left
    0.5f, 0.5f, 0.0f,       1.0f, 1.0f,     // top right
    0.5f, -0.5f, 0.0f,      1.0f, 0.0f,     // bottom right
    -0.5f, -0.5f, 0.0f,     0.0f, 0.0f,     // bottom left
};
unsigned int indices[] = {
    0, 1, 2,    // first triangle
    0, 2, 3     // second triangle
};

Rectangle::Rectangle(std::string texturePath) : texturePath(texturePath)
{
    texture = std::make_unique<Texture>();
    glGenVertexArrays(1, &vaoId);
    glGenBuffers(1, &vboId);
    glGenBuffers(1, &eboId);
}

Rectangle::~Rectangle()
{
    glDeleteVertexArrays(1, &vaoId);
    glDeleteBuffers(1, &vboId);
    glDeleteBuffers(1, &eboId);
}

void Rectangle::load()
{
    texture->load(texturePath);

    glBindVertexArray(vaoId);

    // Load vertices and element indices
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 

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

void Rectangle::draw()
{
    texture->use();
    glBindVertexArray(vaoId);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
