#pragma once

#include "obj.hpp"
#include "types.hpp"

namespace orc
{
    class Regular : public Obj
    {
        public:
        Regular(Id materialId);

        Id GetMaterialId() const;

        private:
        Id materialId;
    };
}
