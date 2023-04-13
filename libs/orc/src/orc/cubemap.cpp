#include <string>
#include <glad/glad.h>
#include "cubemap.hpp"
#include "image.hpp"

namespace orc {
    static void loadTextureFromImage(GLenum target, const std::string &path)
    {
        Image image(path);
        if (image.GetChannels() != 3)
        {
            throw std::runtime_error("Only RGB textures are supported for cubemaps");
        }

        glTexImage2D(target, 0, GL_RGB, image.GetWidth(), image.GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, image.GetData());
    }

    Cubemap::Cubemap(
        const std::string &right,
        const std::string &left,
        const std::string &top,
        const std::string &bottom,
        const std::string &front,
        const std::string &back
    )
    {
        Bind(GL_TEXTURE_CUBE_MAP);

        // Clamp to edge instead of repeat - sampling the seams of the cube will
        // produce the same color as the edge of the corresponding texture
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        loadTextureFromImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X, right);
        loadTextureFromImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, left);
        loadTextureFromImage(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, top);
        loadTextureFromImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, bottom);
        loadTextureFromImage(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, front);
        loadTextureFromImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, back);
    }

    void Cubemap::Use()
    {
        Bind(GL_TEXTURE_CUBE_MAP);
    }

    int64_t Cubemap::GetRenderSortKey() const
    {
        // Arbitrary sort key, won't be used long term
        return 1000;
    }

    CubemapRef::CubemapRef(
        std::string right,
        std::string left,
        std::string top,
        std::string bottom,
        std::string front,
        std::string back
    ) : paths{right, left, top, bottom, front, back} { }

    Texture &CubemapRef::Load() {
        if (!cubemap)
        {
            cubemap = std::make_unique<Cubemap>(
                paths[0], paths[1], paths[2], paths[3], paths[4], paths[5]
            );
        }

        return *cubemap;
    }
}
