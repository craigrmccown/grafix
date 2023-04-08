#pragma once

#include <glm/glm.hpp>

namespace orc
{
    struct Phong
    {
        float Ambient, Diffuse, Specular;
    };

    struct GlobalLight
    {
        glm::vec3 Direction;

        glm::vec3 Color;

        Phong Phong;
    };
}
