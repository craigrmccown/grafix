#pragma once

#include <string>
#include "texture.hpp"

namespace orc
{
    // TODO: Manage this via unified cache with other resources
    class Cubemap : public Texture
    {
        public:
        Cubemap(
            const std::string &right,
            const std::string &left,
            const std::string &top,
            const std::string &bottom,
            const std::string &front,
            const std::string &back
        );

        void Use() override;

        int64_t GetRenderSortKey() const override;
    };

    class CubemapRef : public TextureRef
    {
        public:
        CubemapRef(
            std::string right,
            std::string left,
            std::string top,
            std::string bottom,
            std::string front,
            std::string back
        );

        Texture &Load() override;

        private:
        const std::string paths[6];

        // Each ref will load and unload the cubemap upon load/destruction. Will
        // eventually be managed by a resource manager.
        std::unique_ptr<Cubemap> cubemap;
    };
}
