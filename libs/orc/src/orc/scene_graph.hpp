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

        Id Add(Id parentId, Obj obj);

        void Remove(Id nodeId);

        bool Has(Id nodeId) const;

        Obj &Get(Id nodeId);

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
