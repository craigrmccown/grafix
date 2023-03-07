#include <memory>
#include <queue>
#include "obj.hpp"
#include "scene.hpp"

namespace orc
{
    Scene::Scene()
        : root(std::make_shared<Obj>())
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
        // TODO: Implement Obj iterator instead of BFS + GetChildren
        std::queue<Obj *> q;
        q.push(root.get());

        while (!q.empty())
        {
            Obj *o = q.front();
            o->ComputeMxs();
            q.pop();

            for (const std::shared_ptr<Obj> &c : o->GetChildren())
            {
                q.push(c.get());
            }
        }
    }
}
