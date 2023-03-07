#pragma once

#include <cstdint>
#include <glm/glm.hpp>

namespace orc
{
    struct Phong
    {
        float Ambient, Diffuse, Specular;
    };

    struct Material
    {
        Phong Phong;

        float Shininess;
    };

    struct Attenuation
    {
        float Constant, Linear, Quadratic;
    };
}
