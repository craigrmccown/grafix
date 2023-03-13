#pragma once

#include <functional>
#include <memory>
#include <glm/glm.hpp>
#include "camera.hpp"
#include "mesh.hpp"
#include "obj.hpp"
#include "shader.hpp"
#include "visitor.hpp"

namespace orc
{
    class Scene
    {
        public:
        Scene(std::string dataDir);

        Obj &GetRoot();

        Camera &GetCamera();

        void Update();

        void Draw();

        private:
        class Root : public Obj
        {
            void Dispatch(ObjVisitor &visitor) override;
        };

        std::shared_ptr<Root> root;
        std::shared_ptr<Camera> camera;
        std::unique_ptr<OpenGLShader> regularShader, lightShader;

        // TODO: API to set global light properties
        GlobalLight globalLight;

        // Temporary. Each object should reference its own mesh.
        std::unique_ptr<Mesh> mesh;

        void Traverse(std::function<void(Obj&)>);
    };
}
