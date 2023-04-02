#include <string>
#include <glad/glad.h>
#include "image.hpp"
#include "texture.hpp"

const GLint rgToGrayscaleSwizzleMask[] = { GL_RED, GL_RED, GL_RED, GL_GREEN };

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

        // Load image file and pass to OpenGL. If the image has 1 or 2 color
        // channels, interpret as grayscale by swizzling (rrr for 1 channel,
        // rrrg for 2 channels). Assume 2 channels is grayscale + alpha.
        Image image(path);
        GLenum internalFormat, format;
        switch(image.GetChannels())
        {
            case 1:
                glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, rgToGrayscaleSwizzleMask);
                internalFormat = GL_RGB;
                format = GL_RED;
                break;
            case 2:
                glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, rgToGrayscaleSwizzleMask);
                internalFormat = GL_RGBA;
                format = GL_RG;
                break;
            case 3:
                internalFormat = format = GL_RGB;
                break;
            case 4:
                internalFormat = format = GL_RGBA;
                break;
            default:
                throw std::runtime_error("Image not supported, must have 1-4 color channels");
        }

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, image.GetWidth(), image.GetHeight(), 0, format, GL_UNSIGNED_BYTE, image.GetData());

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
