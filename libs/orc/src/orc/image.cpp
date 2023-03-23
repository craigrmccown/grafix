#include <string>
#include <stdexcept>
#include <glad/glad.h>
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

    const unsigned char *Image::GetData() const {
        return data;
    }

    int Image::GetWidth() const
    {
        return width;
    }

    int Image::GetHeight() const
    {
        return height;
    }

    int Image::GetChannels() const
    {
        return channels;
    }

    GLenum Image::GetFormat() const
    {
        if (GetChannels() == 3) return GL_RGB;
        if (GetChannels() == 4) return GL_RGBA;

        throw std::runtime_error("Image must be RGB or RGBA");
    }
}
