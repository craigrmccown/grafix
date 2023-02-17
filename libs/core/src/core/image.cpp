#include <string>
#include <stdexcept>
#include <stb/stb_image.h>
#include "image.hpp"

Image::Image(std::string path)
{
    data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!data)
    {
        stbi_image_free(data);
        throw std::runtime_error("Failed to load image at " + path);
    }
}

Image::~Image()
{
    stbi_image_free(data);
}

const unsigned char *Image::getData() {
    return data;
}

int Image::getWidth()
{
    return width;
}

int Image::getHeight()
{
    return height;
}

int Image::getChannels()
{
    return channels;
}
