#pragma once

#include <glm/glm.hpp>

namespace orc
{
    struct Phong
    {
        float Ambient, Diffuse, Specular;
    };

    struct Material
    {
        float Shininess;
    };

    struct Attenuation
    {
        float Constant, Linear, Quadratic;
    };

    struct GlobalLight
    {
        glm::vec3 Direction;

        glm::vec3 Color;

        Phong Phong;
    };
}
