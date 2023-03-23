#include <string>
#include <glad/glad.h>
#include "image.hpp"
#include "texture.hpp"

namespace orc
{
    Texture::Texture(std::string path)
    {
        // TODO: Implement OpenGL RAII library to prevent leaks on error
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);

        // Set texture render params
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Load image file and pass to OpenGL
        // TODO: Use appropriate image format (e.g. GL_RGB) based on image file. Currently expects PNG with alpha channel.
        Image image(path);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.GetWidth(), image.GetHeight(), 0, image.GetFormat(), GL_UNSIGNED_BYTE, image.GetData());

        // Auto-generate lower resolution versions of texture to render at distance
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    Texture::~Texture()
    {
        glDeleteTextures(1, &id);
    }

    void Texture::Use()
    {
        glBindTexture(GL_TEXTURE_2D, id);
    }

    unsigned int Texture::GetId()
    {
        return id;
    }
}
