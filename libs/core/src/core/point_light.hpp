#pragma once

#include <glm/glm.hpp>

// TODO: Abstract game object class hierarchy to share common traits, e.g.
// positioning and visibility

/**
 * A light that shines radially from a single point.
 */
class PointLight
{
    public:
    // Creates a white point light at the origin. Use method chaining to set
    // light properties.
    PointLight();

    PointLight setPosition(glm::vec3 pos);

    PointLight randColor();

    PointLight hide();

    PointLight show();

    glm::vec3 getPosition() const;

    glm::vec3 getColor() const;

    bool isHidden() const;

    private:
    glm::vec3 position;
    glm::vec3 color;
    bool hidden;
};
