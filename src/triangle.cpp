#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "triangle.hpp"

float vertices[] = {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.0f, 0.5f, 0.0f,
};

void Triangle::load()
{
    // create and bind VAO
    glGenVertexArrays(1, &vaoId);  
    glBindVertexArray(vaoId);

    // allocate buffer and load vertex data
    unsigned int bufId;
    glGenBuffers(1, &bufId);
    glBindBuffer(GL_ARRAY_BUFFER, bufId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // set vertex attribute to be interpreted by shader:
    // attribute location, number of elements, data type, whether to normalize,
    // stride to next element, offset
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);  
}

void Triangle::draw()
{
    glBindVertexArray(vaoId);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
