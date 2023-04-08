#pragma once

#include <memory>
#include <vector>
#include "mesh.hpp"
#include "node.hpp"
#include "texture_manager.hpp"
#include "types.hpp"
#include "visitor.hpp"

namespace orc
{
    class Object : public Node
    {
        public:
        static std::shared_ptr<Object> Create();

        void Dispatch(NodeVisitor &visitor) override;

        void AddMesh(std::unique_ptr<Mesh> mesh);

        void Draw();

        protected:
        Object();

        private:
        std::vector<std::unique_ptr<Mesh>> meshes;
    };
}
