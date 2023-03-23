#pragma once

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

        protected:
        Object();
    };
}
