#include <stdexcept>
#include <string>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "scene_graph.hpp"
#include "types.hpp"

namespace orc
{
    const Id SceneGraph::Root;

    SceneGraph::SceneGraph()
    {
        nodes[SceneGraph::Root] = Node{.NodeId = SceneGraph::Root};
    }

    Id SceneGraph::Add(Id parentId, Obj obj)
    {
        AssertExists(parentId);

        Id nodeId = NewId();
        Node node = Node{.NodeId = nodeId, .Obj = obj};
        nodes[nodeId] = node;
        nodes[parentId].Children.insert(nodeId);

        return nodeId;
    }

    void SceneGraph::Remove(Id nodeId)
    {
        AssertExists(nodeId);

        for (Id childId : nodes[nodeId].Children)
        {
            Remove(childId);
        }

        nodes.erase(nodeId);
    }

    bool SceneGraph::Has(Id nodeId) const
    {
        return nodes.count(nodeId);
    }

    Obj &SceneGraph::Get(Id nodeId)
    {
        AssertExists(nodeId);

        return nodes[nodeId].Obj;
    }

    void SceneGraph::ComputeMxs()
    {
        ComputeMxsSubtree(SceneGraph::Root, glm::mat4(1.0f));
    }

    Id SceneGraph::NewId()
    {
        return NextId++;
    }

    void SceneGraph::AssertExists(Id nodeId) const
    {
        if (!Has(nodeId))
        {
            throw std::out_of_range(
                "Scene node with ID " + std::to_string(nodeId) + " does not exist"
            );
        }
    }

    void SceneGraph::ComputeMxsSubtree(Id nodeId, glm::mat4 parentMx)
    {
        AssertExists(nodeId);

        Node &node = nodes[nodeId];
        Obj &obj = node.Obj;

        obj.ComputeMxs(parentMx);
        glm::mat4 modelMx = obj.GetModelMx();

        for (Id childId : node.Children) {
            ComputeMxsSubtree(childId, modelMx);
        }
    }
}
