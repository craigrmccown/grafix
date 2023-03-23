#include "object.hpp"
#include "types.hpp"
#include "visitor.hpp"

namespace orc
{
    std::shared_ptr<Object> Object::Create()
    {
        return std::shared_ptr<Object>(new Object());
    }

    Object::Object() {}

    void Object::Dispatch(NodeVisitor &visitor)
    {
        visitor.VisitObject(this);
    }
}
