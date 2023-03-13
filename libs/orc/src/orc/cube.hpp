#pragma once

#include <memory>
#include "mesh.hpp"

namespace orc
{
    std::unique_ptr<Mesh> BuildCubeMesh(std::string dataDir);
}
