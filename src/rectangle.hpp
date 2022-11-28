#pragma once

#include <memory>
#include <string>
#include "texture.hpp"

class Rectangle
{
    public:
    Rectangle(std::string texturePath);

    ~Rectangle();

    void load();

    void draw();

    private:
    std::unique_ptr<Texture> texture;
    std::string texturePath;
    
    // Vertex Array Object
    unsigned int vaoId;

    // Vertex Buffer Object
    unsigned int vboId;

    // Element Buffer Object
    unsigned int eboId;
};
