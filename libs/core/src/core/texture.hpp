#pragma once

#include <string>

class Texture
{
    public:
    Texture();

    ~Texture();

    void load(std::string path);

    void use();

    unsigned int getId();

    private:
    unsigned int id;
};
