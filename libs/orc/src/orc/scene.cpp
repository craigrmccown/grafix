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
#include "object.hpp"
#include "scene.hpp"
#include "shader.hpp"
#include "shaders/light.frag.hpp"
#include "shaders/light.vert.hpp"
#include "shaders/object.frag.hpp"
#include "shaders/object.vert.hpp"
#include "stateful_visitor.hpp"
#include "types.hpp"
#include "visitor.hpp"

const size_t maxOmniLights = 4;

namespace orc
{
    Scene::Scene(std::string dataDir)
        : root(Node::Create())
        , camera(Camera::Create())
        , mesh(BuildCubeMesh(dataDir))
        , globalLight(GlobalLight{
            .Color = glm::vec3(1),
            .Direction = glm::vec3(0, -1, 0),
            .Phong = Phong{.Ambient=0.15, .Diffuse=0.2, .Specular=0.1}
        })
    {
        objectShader = std::make_unique<OpenGLShader>(std::vector<ShaderSrc>{
            ShaderSrc(GL_VERTEX_SHADER, std::string(shaders::object_vert, sizeof(shaders::object_vert))),
            ShaderSrc(GL_FRAGMENT_SHADER, std::string(shaders::object_frag, sizeof(shaders::object_frag))),
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
        std::vector<Object *> objects = visitor.GetObjects();

        // Draw lights
        lightShader->Use();
        for (const OmniLight *light : omniLights)
        {
            lightShader->SetUniformMat4("transformMx", GetCamera().GetViewProjectionMx() * light->GetModelMx());
            lightShader->SetUniformVec3("lightColor", light->GetColor());
            mesh->Use();
            mesh->Draw();
        }

        // Draw objects
        objectShader->Use();
        objectShader->SetUniformVec3("cameraPosition", GetCamera().GetPosition());
        objectShader->SetUniformVec3("globalLight.color", globalLight.Color);
        objectShader->SetUniformVec3("globalLight.direction", globalLight.Direction);
        objectShader->SetUniformFloat("globalLight.phong.ambient", globalLight.Phong.Ambient);
        objectShader->SetUniformFloat("globalLight.phong.diffuse", globalLight.Phong.Diffuse);
        objectShader->SetUniformFloat("globalLight.phong.specular", globalLight.Phong.Specular);

        for (int i = 0; i < maxOmniLights; i ++)
        {
            if (i >= omniLights.size())
            {
                objectShader->SetUniformVec3Element("omniLights", "color", i, glm::vec3(0));
                objectShader->SetUniformFloatElement("omniLights", "constant", i, 1); // Avoid divide by zero
                continue;
            }

            OmniLight *light = omniLights[i];
            objectShader->SetUniformVec3Element("omniLights", "color", i, light->GetColor());
            objectShader->SetUniformVec3Element("omniLights", "position", i, light->GetPosition());
            objectShader->SetUniformFloatElement("omniLights", "phong.ambient", i, light->GetPhong().Ambient);
            objectShader->SetUniformFloatElement("omniLights", "phong.diffuse", i, light->GetPhong().Diffuse);
            objectShader->SetUniformFloatElement("omniLights", "phong.specular", i, light->GetPhong().Specular);
            objectShader->SetUniformFloatElement("omniLights", "constant", i, light->GetAttenuation().Constant);
            objectShader->SetUniformFloatElement("omniLights", "linear", i, light->GetAttenuation().Linear);
            objectShader->SetUniformFloatElement("omniLights", "quadratic", i, light->GetAttenuation().Quadratic);
        }

        if (spotLight)
        {
            objectShader->SetUniformVec3("spotLight.color", spotLight->GetColor());
            objectShader->SetUniformVec3("spotLight.direction", spotLight->GetFront());
            objectShader->SetUniformVec3("spotLight.position", spotLight->GetPosition());
            objectShader->SetUniformFloat("spotLight.inner", spotLight->GetInnerBlur());
            objectShader->SetUniformFloat("spotLight.outer", spotLight->GetOuterBlur());
            objectShader->SetUniformFloat("spotLight.phong.ambient", spotLight->GetPhong().Ambient);
            objectShader->SetUniformFloat("spotLight.phong.diffuse", spotLight->GetPhong().Diffuse);
            objectShader->SetUniformFloat("spotLight.phong.specular", spotLight->GetPhong().Specular);
        }
        else
        {
            objectShader->SetUniformVec3("spotLight.color", glm::vec3(0));
        }

        for (const Object *object : objects)
        {
            objectShader->SetUniformMat4("transformMx", GetCamera().GetViewProjectionMx() * object->GetModelMx());
            objectShader->SetUniformMat4("modelMx", object->GetModelMx());
            objectShader->SetUniformFloat("material.shininess", object->GetMaterial().Shininess); // TODO: Material management

            mesh->Draw();
        }
    }

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
