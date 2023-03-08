#include "regular.hpp"
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
    void Regular::Dispatch(ObjVisitor &visitor)
    {
        visitor.VisitRegular(this);
    }

    const Material &Regular::GetMaterial() const
    {
        if (!material) return defaultMaterial;
        return *material;
    }

    void Regular::SetMaterial(Material *material)
    {
        this->material = material;
    }
}
