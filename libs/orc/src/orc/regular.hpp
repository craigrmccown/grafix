#pragma once

#include "node.hpp"
#include "types.hpp"
#include "visitor.hpp"

namespace orc
{
    class Regular : public Node
    {
        public:
        void Dispatch(NodeVisitor &visitor) override;

        const Material &GetMaterial() const;

        void SetMaterial(Material *material);

        private:
        Material *material;
    };
}
