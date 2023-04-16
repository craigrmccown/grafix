#include <algorithm>
#include <functional>
#include <memory>
#include <queue>
#include <vector>
#include <glad/glad.h>
#include "cube.hpp"
#include "light.hpp"
#include "node.hpp"
#include "object.hpp"
#include "scene.hpp"
#include "shader.hpp"
#include "shaders/monochrome.frag.hpp"
#include "shaders/monochrome.vert.hpp"
#include "shaders/phong.frag.hpp"
#include "shaders/phong.vert.hpp"
#include "shaders/skybox.frag.hpp"
#include "shaders/skybox.vert.hpp"
#include "skybox.hpp"
#include "stateful_visitor.hpp"
#include "types.hpp"
#include "visitor.hpp"

const size_t maxOmniLights = 4;

namespace orc
{
    using ObjMeshPair = std::pair<Object *, std::shared_ptr<Mesh>>;

    static bool compareObjMeshPairs(ObjMeshPair &a, ObjMeshPair &b)
    {
        return a.second->GetTexture().GetRenderSortKey() < b.second->GetTexture().GetRenderSortKey();
    }

    Scene::Scene()
        : root(Node::Create())
        , camera(Camera::Create())
        , globalLight(GlobalLight{
            .Color = glm::vec3(1),
            .Direction = glm::normalize(glm::vec3(0.25, -1, 0)),
            .Phong = Phong{.Ambient=0.1, .Diffuse=0.4, .Specular=0.3}
        })
    {
        phongShader = std::make_unique<OpenGLShader>(std::vector<ShaderSrc>{
            ShaderSrc(GL_VERTEX_SHADER, std::string(shaders::phong_vert, sizeof(shaders::phong_vert))),
            ShaderSrc(GL_FRAGMENT_SHADER, std::string(shaders::phong_frag, sizeof(shaders::phong_frag))),
        });
        monochromeShader = std::make_unique<OpenGLShader>(std::vector<ShaderSrc>{
            ShaderSrc(GL_VERTEX_SHADER, std::string(shaders::monochrome_vert, sizeof(shaders::monochrome_vert))),
            ShaderSrc(GL_FRAGMENT_SHADER, std::string(shaders::monochrome_frag, sizeof(shaders::monochrome_frag))),
        });
        skyboxShader = std::make_unique<OpenGLShader>(std::vector<ShaderSrc>{
            ShaderSrc(GL_VERTEX_SHADER, std::string(shaders::skybox_vert, sizeof(shaders::skybox_vert))),
            ShaderSrc(GL_FRAGMENT_SHADER, std::string(shaders::skybox_frag, sizeof(shaders::skybox_frag))),
        });
        root->AttachChild(camera);
    }

    Node &Scene::GetRoot() const
    {
        return *root;
    }

    Camera &Scene::GetCamera() const
    {
        return *camera;
    }

    void Scene::SetSkybox(std::unique_ptr<Skybox> skybox)
    {
        this->skybox.swap(skybox);
    }

    void Scene::Update()
    {
        Traverse([](Node &node) { node.ComputeMxs(); });
    }

    // TODO: Delegate responsibility of mapping values to uniforms
    // TODO: Traverse/compute matrices only when necessary
    // TODO: Batch render instead of individual draw calls
    // TODO: Set uniforms only when data has changed
    // TODO: Maintain sort order of meshes by transparency, z-distance, etc.
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

        // Sort meshes in order of material properties, like transparency
        std::vector<ObjMeshPair> pairs;
        for (Object *obj : objects)
        {
            for (std::shared_ptr<Mesh> mesh : obj->GetMeshes())
            {
                pairs.push_back(std::make_pair(obj, mesh));
            }
        }
        std::sort(pairs.begin(), pairs.end(), compareObjMeshPairs);

        // Draw lights
        monochromeShader->Use();
        for (OmniLight *light : omniLights)
        {
            monochromeShader->SetUniformMat4("u_transformMx", GetCamera().GetViewProjectionMx() * light->GetModelMx());
            monochromeShader->SetUniformVec3("u_color", light->GetColor());

            for (const std::shared_ptr<Mesh> &mesh : light->GetMeshes())
            {
                mesh->Use();
                mesh->Draw();
            }
        }

        // Draw objects
        phongShader->Use();
        phongShader->SetUniformVec3("u_cameraPosition", GetCamera().GetPosition());
        phongShader->SetUniformVec3("u_globalLight.color", globalLight.Color);
        phongShader->SetUniformVec3("u_globalLight.direction", globalLight.Direction);
        phongShader->SetUniformFloat("u_globalLight.phong.ambient", globalLight.Phong.Ambient);
        phongShader->SetUniformFloat("u_globalLight.phong.diffuse", globalLight.Phong.Diffuse);
        phongShader->SetUniformFloat("u_globalLight.phong.specular", globalLight.Phong.Specular);

        for (int i = 0; i < maxOmniLights; i ++)
        {
            if (i >= omniLights.size())
            {
                phongShader->SetUniformVec3Element("u_omniLights", "color", i, glm::vec3(0));
                phongShader->SetUniformFloatElement("u_omniLights", "constant", i, 1); // Avoid divide by zero
                continue;
            }

            OmniLight *light = omniLights[i];
            phongShader->SetUniformVec3Element("u_omniLights", "color", i, light->GetColor());
            phongShader->SetUniformVec3Element("u_omniLights", "position", i, light->GetPosition());
            phongShader->SetUniformFloatElement("u_omniLights", "phong.ambient", i, light->GetPhong().Ambient);
            phongShader->SetUniformFloatElement("u_omniLights", "phong.diffuse", i, light->GetPhong().Diffuse);
            phongShader->SetUniformFloatElement("u_omniLights", "phong.specular", i, light->GetPhong().Specular);
            phongShader->SetUniformFloatElement("u_omniLights", "brightness", i, light->GetBrightness());
        }

        if (spotLight)
        {
            phongShader->SetUniformVec3("u_spotLight.color", spotLight->GetColor());
            phongShader->SetUniformVec3("u_spotLight.direction", spotLight->GetFront());
            phongShader->SetUniformVec3("u_spotLight.position", spotLight->GetPosition());
            phongShader->SetUniformFloat("u_spotLight.inner", spotLight->GetInnerBlur());
            phongShader->SetUniformFloat("u_spotLight.outer", spotLight->GetOuterBlur());
            phongShader->SetUniformFloat("u_spotLight.phong.ambient", spotLight->GetPhong().Ambient);
            phongShader->SetUniformFloat("u_spotLight.phong.diffuse", spotLight->GetPhong().Diffuse);
            phongShader->SetUniformFloat("u_spotLight.phong.specular", spotLight->GetPhong().Specular);
        }
        else
        {
            phongShader->SetUniformVec3("u_spotLight.color", glm::vec3(0));
        }

        for (ObjMeshPair pair : pairs)
        {
            const Object *object = pair.first;
            const std::shared_ptr<Mesh> mesh = pair.second;

            phongShader->SetUniformMat4("u_transformMx", GetCamera().GetViewProjectionMx() * object->GetModelMx());
            phongShader->SetUniformMat4("u_modelMx", object->GetModelMx());

            mesh->Use();
            mesh->Draw();
        }

        if (skybox)
        {
            skyboxShader->Use();

            // Skybox never moves, so the model matrix will always be the
            // identity. We also remove the translation component of the view
            // matrix since the skybox shouldn't be influenced by camera
            // position.
            glm::mat4 skyboxMx = glm::mat4(glm::mat3(GetCamera().GetViewMx()));
            skyboxMx = GetCamera().GetProjectionMx() * skyboxMx;

            skyboxShader->SetUniformMat4("u_transformMx", skyboxMx);
            skybox->Use();
            skybox->Draw();
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
