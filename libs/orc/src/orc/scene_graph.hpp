#pragma once

#include <map>
#include <set>
#include <glm/glm.hpp>
#include "obj.hpp"
#include "types.hpp"

namespace orc
{
    class SceneGraph
    {
        public:
        static const Id Root = 0;

        SceneGraph();

        // Adds an object to the scene graph. A parent ID can be supplied to
        // establish a parent-child relationship between two objects.
        // Transformations applied to a parent object forms the coordinate space
        // for its children.
        Id Add(Id parentId, Obj obj);

        // Removes an object from the graph. If the object does not exist, an
        // exception is thrown.
        void Remove(Id nodeId);

        // Returns true iff the identified object exists.
        bool Has(Id nodeId) const;

        // Returns a reference to the identified object. If the object does not
        // exist, an exception is thrown.
        Obj &Get(Id nodeId);

        // Computes the transformation matrices for every object in the graph.
        // This is an expensive operation that should only be called once per
        // frame, if possible.
        void ComputeMxs();

        private:
        static inline Id NextId = 1;

        static Id NewId();

        struct Node
        {
            Id NodeId;
            Obj Obj;
            std::set<Id> Children;
        };

        std::map<Id, Node> nodes;

        void AssertExists(Id nodeId) const;

        void ComputeMxsSubtree(Id nodeId, glm::mat4 parentMx);
    };
}
