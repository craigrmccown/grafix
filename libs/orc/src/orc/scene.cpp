#include <memory>
#include <queue>
#include "obj.hpp"
#include "scene.hpp"

namespace orc
{
    Scene::Scene() : root(std::make_shared<Obj>()) { }

    Obj &Scene::GetRoot()
    {
        return *root;
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
