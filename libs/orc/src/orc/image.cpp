#include <string>
#include <stdexcept>
#include <stb/stb_image.h>
#include "image.hpp"

namespace orc
{
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

    const unsigned char *Image::GetData() {
        return data;
    }

    int Image::GetWidth()
    {
        return width;
    }

    int Image::GetHeight()
    {
        return height;
    }

    int Image::GetChannels()
    {
        return channels;
    }
}
