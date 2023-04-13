#pragma once

#include <memory>
#include <string>
#include "cubemap.hpp"
#include "mesh.hpp"

namespace orc
{
    class Skybox : public Mesh
    {
        public:
        Skybox(std::unique_ptr<CubemapRef> cubemap);

        void Draw() override;
    };
}
