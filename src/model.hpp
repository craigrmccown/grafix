#pragma once

#include <memory>
#include <string>
#include <vector>
#include "texture.hpp"

class Model
{
    public:
    Model(std::vector<float> vertices, std::string texturePath);

    ~Model();

    void load();

    void draw();

    private:
    std::unique_ptr<Texture> texture;
    std::string texturePath;
    
    // Vertex Array Object
    unsigned int vaoId;

    // Vertex Buffer Object
    unsigned int vboId;

    // Element Buffer Object (not currently used)
    unsigned int eboId;

    std::vector<float> vertices;
};
