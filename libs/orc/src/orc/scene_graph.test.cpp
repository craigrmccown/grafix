#include <iostream>
#include <limits>
#include <string>
#include <catch2/catch_test_macros.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
#include "scene_graph.hpp"

bool compareVec3(glm::vec3 a, glm::vec3 b) {
    return glm::all(glm::epsilonEqual(a, b, std::numeric_limits<float>::epsilon()));
}

TEST_CASE("Basic data structure operations", "[orc]") {
    orc::SceneGraph graph;
    REQUIRE(graph.Has(orc::SceneGraph::Root));

    orc::Id id = graph.Add(orc::SceneGraph::Root, orc::Obj());
    REQUIRE(graph.Has(id));
    REQUIRE_NOTHROW(graph.Get(id));
    REQUIRE_NOTHROW(graph.Remove(id));
    REQUIRE_FALSE(graph.Has(id));
    REQUIRE_THROWS(graph.Get(id));
    REQUIRE_THROWS(graph.Remove(id));
}

TEST_CASE("Orient single object", "[orc]") {
    orc::SceneGraph graph;
    orc::Id id = graph.Add(orc::SceneGraph::Root, orc::Obj());

    // Start at origin facing +Z, rotate 90 degrees clockwise around Y axis, and
    // move away from the origin
    graph.Get(id).Translate(2.0f, 1.0f, 4.0f);
    graph.Get(id).Rotate(glm::radians(90.0f), 0.0f, 0.0f);
    graph.ComputeMxs();

    REQUIRE(compareVec3(graph.Get(id).GetPosition(), glm::vec3(2.0f, 1.0f, 4.0f)));
    REQUIRE(compareVec3(graph.Get(id).GetUp(), glm::vec3(0.0f, 1.0f, 0.0f)));
    REQUIRE(compareVec3(graph.Get(id).GetRight(), glm::vec3(0.0f, 0.0f, -1.0f)));
    REQUIRE(compareVec3(graph.Get(id).GetFront(), glm::vec3(1.0f, 0.0f, 0.0f)));
}

TEST_CASE("Orient child object", "[orc]") {
    orc::SceneGraph graph;
    orc::Id parentId = graph.Add(orc::SceneGraph::Root, orc::Obj());
    orc::Id childId = graph.Add(parentId, orc::Obj());

    // Move child away from parent, rotate parent clockwise around Z axis, move
    // parent away from origin
    graph.Get(childId).Translate(2.0f, 1.0f, 4.0f);
    graph.Get(parentId).Translate(-10.0f, 7.0f, -5.0f);
    graph.Get(parentId).Rotate(0.0f, 0.0f, glm::radians(90.0f));
    graph.ComputeMxs();

    REQUIRE(compareVec3(graph.Get(childId).GetPosition(), glm::vec3(-11.0f, 9.0f, -1.0f)));
    REQUIRE(compareVec3(graph.Get(childId).GetUp(), glm::vec3(-1.0f, 0.0f, 0.0f)));
    REQUIRE(compareVec3(graph.Get(childId).GetRight(), glm::vec3(0.0f, 1.0f, 0.0f)));
    REQUIRE(compareVec3(graph.Get(childId).GetFront(), glm::vec3(0.0f, 0.0f, 1.0f)));
}
