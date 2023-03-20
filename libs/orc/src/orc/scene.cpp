#include <algorithm>
#include <functional>
#include <memory>
#include <queue>
#include <vector>
#include <glad/glad.h>
#include "cube.hpp"
#include "light.hpp"
#include "mesh.hpp"
#include "node.hpp"
#include "regular.hpp"
#include "scene.hpp"
#include "shader.hpp"
#include "shaders/light.frag.hpp"
#include "shaders/light.vert.hpp"
#include "shaders/regular.frag.hpp"
#include "shaders/regular.vert.hpp"
#include "stateful_visitor.hpp"
#include "types.hpp"
#include "visitor.hpp"

const size_t maxOmniLights = 4;

namespace orc
{
    Scene::Scene(std::string dataDir)
        : root(std::make_shared<Root>())
        , camera(std::make_shared<Camera>())
        , mesh(BuildCubeMesh(dataDir))
        , globalLight(GlobalLight{
            .Color = glm::vec3(1),
            .Direction = glm::vec3(0, -1, 0),
            .Phong = Phong{.Ambient=0.15, .Diffuse=0.2, .Specular=0.1}
        })
    {
        regularShader = std::make_unique<OpenGLShader>(std::vector<ShaderSrc>{
            ShaderSrc(GL_VERTEX_SHADER, std::string(shaders::regular_vert, sizeof(shaders::regular_vert))),
            ShaderSrc(GL_FRAGMENT_SHADER, std::string(shaders::regular_frag, sizeof(shaders::regular_frag))),
        });
        lightShader = std::make_unique<OpenGLShader>(std::vector<ShaderSrc>{
            ShaderSrc(GL_VERTEX_SHADER, std::string(shaders::light_vert, sizeof(shaders::light_vert))),
            ShaderSrc(GL_FRAGMENT_SHADER, std::string(shaders::light_frag, sizeof(shaders::light_frag))),
        });
        root->AttachChild(camera);
        mesh->Use();
    }

    Node &Scene::GetRoot()
    {
        return *root;
    }

    Camera &Scene::GetCamera()
    {
        return *camera;
    }

    void Scene::Update()
    {
        Traverse([](Node &node) { node.ComputeMxs(); });
    }

    // TODO: Delegate responsibility of mapping values to uniforms
    // TODO: Traverse/compute matrices only when necessary
    // TODO: Batch render instead of individual draw calls
    // TODO: Set uniforms only when data has changed
    void Scene::Draw()
    {
        // Collect all nodes in scene graph and separate by type
        StatefulVisitor visitor;
        Traverse([&visitor](Node &node) { node.Dispatch(visitor); });

        // Extract drawable nodes
        std::vector<OmniLight*> omniLights = std::vector<OmniLight *>(
            visitor.GetOmniLights().begin(),
            visitor.GetOmniLights().begin() + std::min(visitor.GetOmniLights().size(), maxOmniLights)
        );
        SpotLight *spotLight = nullptr;
        if (visitor.GetSpotLights().size())
        {
            spotLight = visitor.GetSpotLights()[0];
        }
        std::vector<Regular*> regulars = visitor.GetRegulars();

        // Draw lights
        lightShader->Use();
        for (const OmniLight *light : omniLights)
        {
            lightShader->SetUniformMat4("transformMx", GetCamera().GetViewProjectionMx() * light->GetModelMx());
            lightShader->SetUniformVec3("lightColor", light->GetColor());
            mesh->Use();
            mesh->Draw();
        }

        // Draw regulars
        regularShader->Use();
        regularShader->SetUniformVec3("cameraPosition", GetCamera().GetPosition());
        regularShader->SetUniformVec3("globalLight.color", globalLight.Color);
        regularShader->SetUniformVec3("globalLight.direction", globalLight.Direction);
        regularShader->SetUniformFloat("globalLight.phong.ambient", globalLight.Phong.Ambient);
        regularShader->SetUniformFloat("globalLight.phong.diffuse", globalLight.Phong.Diffuse);
        regularShader->SetUniformFloat("globalLight.phong.specular", globalLight.Phong.Specular);

        // TODO: Material management
        regularShader->SetUniformFloat("material.shininess", 64.0f);

        for (int i = 0; i < maxOmniLights; i ++)
        {
            if (i >= omniLights.size())
            {
                regularShader->SetUniformVec3Element("omniLights", "color", i, glm::vec3(0));
                regularShader->SetUniformFloatElement("omniLights", "constant", i, 1); // Avoid divide by zero
                continue;
            }

            OmniLight *light = omniLights[i];
            regularShader->SetUniformVec3Element("omniLights", "color", i, light->GetColor());
            regularShader->SetUniformVec3Element("omniLights", "position", i, light->GetPosition());
            regularShader->SetUniformFloatElement("omniLights", "phong.ambient", i, light->GetPhong().Ambient);
            regularShader->SetUniformFloatElement("omniLights", "phong.diffuse", i, light->GetPhong().Diffuse);
            regularShader->SetUniformFloatElement("omniLights", "phong.specular", i, light->GetPhong().Specular);
            regularShader->SetUniformFloatElement("omniLights", "constant", i, light->GetAttenuation().Constant);
            regularShader->SetUniformFloatElement("omniLights", "linear", i, light->GetAttenuation().Linear);
            regularShader->SetUniformFloatElement("omniLights", "quadratic", i, light->GetAttenuation().Quadratic);
        }

        if (spotLight)
        {
            regularShader->SetUniformVec3("spotLight.color", spotLight->GetColor());
            regularShader->SetUniformVec3("spotLight.direction", spotLight->GetFront());
            regularShader->SetUniformVec3("spotLight.position", spotLight->GetPosition());
            regularShader->SetUniformFloat("spotLight.inner", spotLight->GetInnerBlur());
            regularShader->SetUniformFloat("spotLight.outer", spotLight->GetOuterBlur());
            regularShader->SetUniformFloat("spotLight.phong.ambient", spotLight->GetPhong().Ambient);
            regularShader->SetUniformFloat("spotLight.phong.diffuse", spotLight->GetPhong().Diffuse);
            regularShader->SetUniformFloat("spotLight.phong.specular", spotLight->GetPhong().Specular);
        }
        else
        {
            regularShader->SetUniformVec3("spotLight.color", glm::vec3(0));
        }

        for (const Regular *regular : regulars)
        {
            regularShader->SetUniformMat4("transformMx", GetCamera().GetViewProjectionMx() * regular->GetModelMx());
            regularShader->SetUniformMat4("modelMx", regular->GetModelMx());
            mesh->Draw();
        }
    }

    // No-op
    void Scene::Root::Dispatch(NodeVisitor &visitor) {}

    void Scene::Traverse(std::function<void(Node&)> f)
    {
        std::queue<Node *const> q;
        q.push(root.get());

        while (!q.empty())
        {
            Node *o = q.front();
            f(*o);
            q.pop();

            for (const std::shared_ptr<Node> &c : o->GetChildren())
            {
                q.push(c.get());
            }
        }
    }
}
