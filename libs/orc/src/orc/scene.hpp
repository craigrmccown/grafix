#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "camera.hpp"
#include "obj.hpp"

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
        std::shared_ptr<Obj> root;
        std::shared_ptr<Camera> camera;
    };
}
