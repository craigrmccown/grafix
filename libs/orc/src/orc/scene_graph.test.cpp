#include <catch2/catch_test_macros.hpp>
#include "scene_graph.hpp"

TEST_CASE("Trivial test case", "[orc]") {
    orc::SceneGraph graph;
    REQUIRE(graph.Has(orc::SceneGraph::Root));
}
