#include <memory>
#include <queue>
#include "light.hpp"
#include "obj.hpp"
#include "regular.hpp"
#include "scene.hpp"
#include "stateful_visitor.hpp"
#include "visitor.hpp"

namespace orc
{
    Scene::Scene()
        : root(std::make_shared<Root>())
        , camera(std::make_shared<Camera>(45.0, 16.0/9.0))
    {
        root->AttachChild(camera);
    }

    Obj &Scene::GetRoot()
    {
        return *root;
    }

    Camera &Scene::GetCamera()
    {
        return *camera;
    }

    void Scene::Update()
    {
        StatefulVisitor visitor;

        // TODO: Implement Obj iterator instead of BFS + GetChildren
        std::queue<Obj *const> q;
        q.push(root.get());

        while (!q.empty())
        {
            Obj *o = q.front();
            q.pop();

            o->ComputeMxs();
            o->Dispatch(visitor);

            for (const std::shared_ptr<Obj> &c : o->GetChildren())
            {
                q.push(c.get());
            }
        }

        // Set uniforms
    }

    // No-op
    void Scene::Root::Dispatch(ObjVisitor &visitor) {}
}
