#include <memory>
#include <queue>
#include <vector>
#include "light.hpp"
#include "obj.hpp"
#include "regular.hpp"
#include "scene.hpp"
#include "stateful_visitor.hpp"
#include "visitor.hpp"

// TODO: Improve handling of paths (x-platform and relative path root)
const std::string shaderSourceDir = "../../../libs/orc/src/orc/shaders";

static std::string buildShaderSrcPath(std::string filename)
{
    return shaderSourceDir + "/" + filename;
}

namespace orc
{
    Scene::Scene()
        : root(std::make_shared<Root>())
        , camera(std::make_shared<Camera>(45.0, 16.0/9.0))
    {
        root->AttachChild(camera);
        regularShader = LoadShaderFromFiles(std::vector<std::string>{
            buildShaderSrcPath("regular.vert"),
            buildShaderSrcPath("regular.frag")
        });
        lightShader = LoadShaderFromFiles(std::vector<std::string>{
            buildShaderSrcPath("light.vert"),
            buildShaderSrcPath("light.frag")
        });
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
        Traverse([](Obj &obj) { obj.ComputeMxs(); });
    }

    // No-op
    void Scene::Root::Dispatch(ObjVisitor &visitor) {}

    void Scene::Traverse(void (*f)(Obj &obj))
    {
        std::queue<Obj *const> q;
        q.push(root.get());

        while (!q.empty())
        {
            Obj *o = q.front();
            f(*o);
            q.pop();

            for (const std::shared_ptr<Obj> &c : o->GetChildren())
            {
                q.push(c.get());
            }
        }
    }
}
