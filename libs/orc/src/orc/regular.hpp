#pragma once

#include "obj.hpp"
#include "types.hpp"

namespace orc
{
    class Regular : public Obj
    {
        public:
        const Material &GetMaterial() const;

        void SetMaterial(Material *material);

        private:
        Material *material;
    };
}
