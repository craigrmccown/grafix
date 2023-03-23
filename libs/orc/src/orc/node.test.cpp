#include <iomanip>
#include <iostream>
#include <sstream>
#include <memory>
#include <catch2/catch_test_macros.hpp>
#include <testutils/glm.hpp>
#include "node.hpp"

TEST_CASE("Set transformation matrix", "[orc]")
{
    std::shared_ptr<orc::Node> n1 = orc::Node::Create();
    n1->Translate(3, 5, -1);
    n1->Rotate(0.134, -1.453, 0.897);
    n1->ComputeMxs();

    std::shared_ptr<orc::Node> n2 = orc::Node::Create();
    n2->SetTransformMx(n1->GetModelMx());
    n2->ComputeMxs();

    REQUIRE(testutils::Mat4Equals(n1->GetModelMx(), n2->GetModelMx()));
}
