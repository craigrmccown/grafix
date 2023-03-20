#include "object.hpp"
#include "types.hpp"
#include "visitor.hpp"

const orc::Material defaultMaterial = orc::Material{
    .Phong = orc::Phong{
        .Ambient = 0.05f,
        .Diffuse = 0.25f,
        .Specular = 0.25f
    },
    .Shininess = 64.0f
};

namespace orc
{
    void Object::Dispatch(NodeVisitor &visitor)
    {
        visitor.VisitObject(this);
    }

    const Material &Object::GetMaterial() const
    {
        if (!material) return defaultMaterial;
        return *material;
    }

    void Object::SetMaterial(Material *material)
    {
        this->material = material;
    }
}
