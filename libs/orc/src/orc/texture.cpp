#include <string>
#include <glad/glad.h>
#include "texture.hpp"

namespace orc
{
    Texture::Texture()
    {
        glGenTextures(1, &id);
    }

    Texture::~Texture()
    {
        glDeleteTextures(1, &id);
    }

    unsigned int Texture::GetId() const
    {
        return id;
    }

    void Texture::Bind(GLenum target)
    {
        glBindTexture(target, id);
    }
}
