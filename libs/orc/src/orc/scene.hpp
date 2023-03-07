#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "obj.hpp"

namespace orc
{
    class Scene
    {
        public:
        Scene();

        Obj &GetRoot();

        void Update();

        private:
        std::shared_ptr<Obj> root;
    };
}
