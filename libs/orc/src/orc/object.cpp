#include <memory>
#include <vector>
#include "mesh.hpp"
#include "object.hpp"
#include "types.hpp"
#include "visitor.hpp"

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

    void Object::AddMesh(std::shared_ptr<Mesh> mesh)
    {
        meshes.push_back(mesh);
    }

    const std::vector<const std::shared_ptr<Mesh>> &Object::GetMeshes() const
    {
        return meshes;
    }
}
