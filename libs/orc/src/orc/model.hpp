#pragma once

#include <memory>
#include "object.hpp"

namespace orc
{
    std::shared_ptr<Object> LoadModel(std::string path);
}
