#pragma once

#include <functional>
#include <memory>
#include <glm/glm.hpp>
#include "camera.hpp"
#include "mesh.hpp"
#include "node.hpp"
#include "shader.hpp"
#include "visitor.hpp"

namespace orc
{
    class Scene
    {
        public:
        Scene(std::string dataDir);

        Node &GetRoot();

        Camera &GetCamera();

        void Update();

        void Draw();

        private:
        class Root : public Node
        {
            void Dispatch(NodeVisitor &visitor) override;
        };

        std::shared_ptr<Root> root;
        std::shared_ptr<Camera> camera;
        std::unique_ptr<OpenGLShader> objectShader, lightShader;

        // TODO: API to set global light properties
        GlobalLight globalLight;

        // Temporary. Each object should reference its own mesh.
        std::unique_ptr<Mesh> mesh;

        void Traverse(std::function<void(Node&)>);
    };
}
