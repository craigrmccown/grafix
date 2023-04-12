#pragma once

#include <memory>
#include <vector>
#include "mesh.hpp"
#include "node.hpp"
#include "types.hpp"
#include "visitor.hpp"

namespace orc
{
    class Object : public Node
    {
        public:
        static std::shared_ptr<Object> Create();

        void Dispatch(NodeVisitor &visitor) override;

        void AddMesh(std::shared_ptr<Mesh> mesh);

        const std::vector<const std::shared_ptr<Mesh>> &GetMeshes() const;

        protected:
        Object();

        private:
        std::vector<const std::shared_ptr<Mesh>> meshes;
    };
}
