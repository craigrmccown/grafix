#include <string>
#include <glad/glad.h>
#include "image.hpp"
#include "texture.hpp"

Texture::Texture()
{
    glGenTextures(1, &id);
}

Texture::~Texture()
{
    glDeleteTextures(1, &id);
}

void Texture::load(std::string path)
{
    glBindTexture(GL_TEXTURE_2D, id);

    // Set texture render params
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load image file and pass to OpenGL
    // TODO: Use appropriate image format (e.g. GL_RGB) based on image file. Currently expects PNG with alpha channel.
    Image image(path);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.getWidth(), image.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getData());

    // Auto-generate lower resolution versions of texture to render at distance
    glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::use()
{
    glBindTexture(GL_TEXTURE_2D, id);
}

unsigned int Texture::getId()
{
    return id;
}
