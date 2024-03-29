#include <string>
#include <glad/glad.h>
#include "image.hpp"
#include "texture_2d.hpp"

const GLint redToGrayscaleSwizzleMask[] = { GL_RED, GL_RED, GL_RED, GL_ONE };
const GLint rgToGrayscaleSwizzleMask[] = { GL_RED, GL_RED, GL_RED, GL_GREEN };
const GLint rgbToRgbaSwizzleMask[] = { GL_RED, GL_GREEN, GL_BLUE, GL_ONE };

namespace orc
{
    Texture2D::Texture2D(Texture2D::Type type, const std::string &path)
    {
        Bind(GL_TEXTURE_2D);

        // Set texture render params
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Load image file and pass to OpenGL.
        Image image(path);
        GLenum internalFormat;

        // Perform gamma correction, only for base-color textures (normal maps,
        // specular maps, etc. are assumed to be defined in linear space).
        if (type == Type::BaseColor) internalFormat = GL_SRGB_ALPHA;
        else internalFormat = GL_RGBA;

        // All textures are represented internally as RGBA, so non-RGBA formats
        // are corrected. If the image has 1 or 2 color channels, interpret as
        // grayscale by swizzling (rrr for 1 channel, rrrg for 2 channels).
        // Assume 2 channels is grayscale + alpha.
        switch(image.GetChannels())
        {
            case 1:
                glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, redToGrayscaleSwizzleMask);
                format = GL_RED;
                break;
            case 2:
                glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, rgToGrayscaleSwizzleMask);
                format = GL_RG;
                break;
            case 3:
                glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, rgbToRgbaSwizzleMask);
                format = GL_RGB;
                break;
            case 4:
                format = GL_RGBA;
                break;
            default:
                throw std::runtime_error("Image not supported, must have 1-4 color channels");
        }

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, image.GetWidth(), image.GetHeight(), 0, format, GL_UNSIGNED_BYTE, image.GetData());

        // Auto-generate lower resolution versions of texture to render at distance
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    void Texture2D::Use()
    {
        Bind(GL_TEXTURE_2D);
    }

    int64_t Texture2D::GetRenderSortKey() const
    {
        return format == GL_RGBA;
    }

    std::map<std::string, std::unique_ptr<Texture2D>> Texture2DRef::cache;

    Texture2DRef::Texture2DRef(Texture2D::Type type, const std::string &path)
        : type(type)
        , path(path)
        {}

    Texture &Texture2DRef::Load()
    {
        if (cache.count(path) == 0)
        {
            cache[path] = std::make_unique<Texture2D>(type, path);
        }

        return *cache[path];
    }
}
