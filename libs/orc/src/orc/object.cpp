#include <memory>
#include <vector>
#include "mesh.hpp"
#include "object.hpp"
#include "texture_manager.hpp"
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

    void Object::AddMesh(std::unique_ptr<Mesh> mesh)
    {
        meshes.push_back(std::move(mesh));
    }

    void Object::Draw(TextureManager &textureManager)
    {
        for (const std::unique_ptr<Mesh> &mesh : meshes)
        {
            mesh->Use(textureManager);
            mesh->Draw();
        }
    }
}
