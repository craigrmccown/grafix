#pragma once

#include "obj.hpp"
#include "types.hpp"
#include "visitor.hpp"

namespace orc
{
    class Regular : public Obj
    {
        public:
        void Dispatch(ObjVisitor &visitor) override;

        const Material &GetMaterial() const;

        void SetMaterial(Material *material);

        private:
        Material *material;
    };
}
