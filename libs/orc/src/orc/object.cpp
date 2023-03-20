#include "object.hpp"
#include "types.hpp"
#include "visitor.hpp"

const orc::Material defaultMaterial = orc::Material{.Shininess = 64.0f};

namespace orc
{
    std::shared_ptr<Object> Object::Create()
    {
        return std::shared_ptr<Object>(new Object());
    }

    Object::Object() {}

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
