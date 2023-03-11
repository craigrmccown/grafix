#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "camera.hpp"
#include "obj.hpp"
#include "shader.hpp"
#include "visitor.hpp"

namespace orc
{
    class Scene
    {
        public:
        Scene();

        Obj &GetRoot();

        Camera &GetCamera();

        void Update();

        private:
        class Root : public Obj
        {
            void Dispatch(ObjVisitor &visitor) override;
        };

        std::shared_ptr<Root> root;
        std::shared_ptr<Camera> camera;
        std::unique_ptr<OpenGLShader> regularShader, lightShader;
    };
}
