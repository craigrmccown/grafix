#include <memory>
#include <catch2/catch_test_macros.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
#include "obj.hpp"
#include "scene.hpp"

bool compareVec3(glm::vec3 a, glm::vec3 b) {
    return glm::all(glm::epsilonEqual(a, b, std::numeric_limits<float>::epsilon()));
}

TEST_CASE("Orient single object", "[orc]") {
    orc::Scene scene;
    std::shared_ptr<orc::Obj> o = std::make_shared<orc::Obj>();

    scene.GetRoot().AttachChild(o);
    
    // Start at origin facing +Z, rotate 90 degrees clockwise around Y axis, and
    // move away from the origin
    o->Translate(2.0f, 1.0f, 4.0f);
    o->Rotate(glm::radians(90.0f), 0.0f, 0.0f);
    scene.Update();

    REQUIRE(compareVec3(o->GetPosition(), glm::vec3(2.0f, 1.0f, 4.0f)));
    REQUIRE(compareVec3(o->GetUp(), glm::vec3(0.0f, 1.0f, 0.0f)));
    REQUIRE(compareVec3(o->GetRight(), glm::vec3(0.0f, 0.0f, -1.0f)));
    REQUIRE(compareVec3(o->GetFront(), glm::vec3(1.0f, 0.0f, 0.0f)));
}

TEST_CASE("Orient child object", "[orc]") {
    orc::Scene scene;
    std::shared_ptr<orc::Obj> parent = std::make_shared<orc::Obj>();
    std::shared_ptr<orc::Obj> child = std::make_shared<orc::Obj>();
    parent->AttachChild(child);
    scene.GetRoot().AttachChild(parent);

    // Move child away from parent, rotate parent clockwise around Z axis, move
    // parent away from origin
    child->Translate(2.0f, 1.0f, 4.0f);
    parent->Translate(-10.0f, 7.0f, -5.0f);
    parent->Rotate(0.0f, 0.0f, glm::radians(90.0f));
    scene.Update();

    REQUIRE(compareVec3(child->GetPosition(), glm::vec3(-11.0f, 9.0f, -1.0f)));
    REQUIRE(compareVec3(child->GetUp(), glm::vec3(-1.0f, 0.0f, 0.0f)));
    REQUIRE(compareVec3(child->GetRight(), glm::vec3(0.0f, 1.0f, 0.0f)));
    REQUIRE(compareVec3(child->GetFront(), glm::vec3(0.0f, 0.0f, 1.0f)));
}
