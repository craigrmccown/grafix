#pragma once

#include <functional>
#include <memory>
#include <glm/glm.hpp>
#include "camera.hpp"
#include "mesh.hpp"
#include "node.hpp"
#include "shader.hpp"
#include "skybox.hpp"
#include "visitor.hpp"

namespace orc
{
    class Scene
    {
        public:
        Scene();

        Node &GetRoot() const;

        Camera &GetCamera() const;

        // Sets the skybox for the scene. If a skybox has already been set, it
        // will be destroyed and must be reloaded.
        void SetSkybox(std::unique_ptr<Skybox> skybox);

        void Update();

        void Draw();

        private:
        std::shared_ptr<Node> root;
        std::shared_ptr<Camera> camera;
        std::unique_ptr<OpenGLShader> objectShader, lightShader, skyboxShader;
        std::unique_ptr<Skybox> skybox;

        // TODO: API to set global light properties
        GlobalLight globalLight;

        void Traverse(std::function<void(Node&)>);
    };
}
