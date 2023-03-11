#include <iostream>
#include <memory>
#include <queue>
#include <vector>
#include <glad/glad.h>
#include "light.hpp"
#include "obj.hpp"
#include "regular.hpp"
#include "scene.hpp"
#include "shader.hpp"
#include "shaders/light.frag.hpp"
#include "shaders/light.vert.hpp"
#include "shaders/regular.frag.hpp"
#include "shaders/regular.vert.hpp"
#include "stateful_visitor.hpp"
#include "visitor.hpp"

namespace orc
{
    Scene::Scene()
        : root(std::make_shared<Root>())
        , camera(std::make_shared<Camera>(45.0, 16.0/9.0))
    {
        root->AttachChild(camera);
        regularShader = std::make_unique<OpenGLShader>(std::vector<ShaderSrc>{
            ShaderSrc(GL_VERTEX_SHADER, std::string(shaders::regular_vert, sizeof(shaders::regular_vert))),
            ShaderSrc(GL_FRAGMENT_SHADER, std::string(shaders::regular_frag, sizeof(shaders::regular_frag))),
        });
        lightShader = std::make_unique<OpenGLShader>(std::vector<ShaderSrc>{
            ShaderSrc(GL_VERTEX_SHADER, std::string(shaders::light_vert, sizeof(shaders::light_vert))),
            ShaderSrc(GL_FRAGMENT_SHADER, std::string(shaders::light_frag, sizeof(shaders::light_frag))),
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
