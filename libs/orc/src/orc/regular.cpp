#include "regular.hpp"
#include "types.hpp"

namespace orc
{
    Regular::Regular(Id materialId) : materialId(materialId) {}

    Id Regular::GetMaterialId() const
    {
        return materialId;
    }
}
