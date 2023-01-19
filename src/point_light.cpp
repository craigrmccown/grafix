#include <glm/glm.hpp>
#include "point_light.hpp"

const glm::vec3 colors[] {
    glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(1.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 1.0f),
    glm::vec3(1.0f, 0.0f, 1.0f),
};

float getRandFraction()
{
    return (float)(rand() % 100) / 100;
}

PointLight::PointLight()
    : position(glm::vec3(0.0))
    , color(glm::vec3(1.0))
    , hidden(false)
    {}

PointLight PointLight::setPosition(glm::vec3 pos)
{
    position = pos;
    return *this;
}

PointLight PointLight::randColor()
{
    color = colors[rand() % (sizeof(colors) / sizeof(glm::vec3))];
    return *this;
}

glm::vec3 PointLight::getPosition() const
{
    return position;
}

glm::vec3 PointLight::getColor() const
{
    if (isHidden()) return glm::vec3(0.0f);
    else return color;
}

PointLight PointLight::hide()
{
    hidden = true;
    return *this;
}

PointLight PointLight::show()
{
    hidden = false;
    return *this;
}

bool PointLight::isHidden() const
{
    return hidden;
}
