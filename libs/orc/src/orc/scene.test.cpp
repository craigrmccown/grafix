#include <memory>
#include <catch2/catch_test_macros.hpp>
#include <glm/glm.hpp>
#include <testutils/glm.hpp>
#include "light.hpp"
#include "object.hpp"
#include "scene.hpp"

TEST_CASE("Orient single object", "[orc]") {
    orc::Scene scene;
    std::shared_ptr<orc::Object> o = orc::Object::Create();

    scene.GetRoot().AttachChild(o);
    
    // Start at origin facing -Z, rotate 90 degrees clockwise around Y axis, and
    // move away from the origin
    o->Translate(2.0f, 1.0f, 4.0f);
    o->Rotate(glm::radians(90.0f), 0.0f, 0.0f);
    scene.Update();

    REQUIRE(testutils::Vec3Equals(o->GetPosition(), glm::vec3(2.0f, 1.0f, 4.0f)));
    REQUIRE(testutils::Vec3Equals(o->GetUp(), glm::vec3(0.0f, 1.0f, 0.0f)));
    REQUIRE(testutils::Vec3Equals(o->GetRight(), glm::vec3(0.0f, 0.0f, -1.0f)));
    REQUIRE(testutils::Vec3Equals(o->GetFront(), glm::vec3(-1.0f, 0.0f, 0.0f)));
}

TEST_CASE("Orient child object", "[orc]") {
    orc::Scene scene;
    std::shared_ptr<orc::Object> parent = orc::Object::Create();
    std::shared_ptr<orc::Object> child = orc::Object::Create();
    parent->AttachChild(child);
    scene.GetRoot().AttachChild(parent);

    // Move child away from parent, rotate parent clockwise around Z axis, move
    // parent away from origin
    child->Translate(2.0f, 1.0f, 4.0f);
    parent->Translate(-10.0f, 7.0f, -5.0f);
    parent->Rotate(0.0f, 0.0f, glm::radians(90.0f));
    scene.Update();

    REQUIRE(testutils::Vec3Equals(child->GetPosition(), glm::vec3(-11.0f, 9.0f, -1.0f)));
    REQUIRE(testutils::Vec3Equals(child->GetUp(), glm::vec3(-1.0f, 0.0f, 0.0f)));
    REQUIRE(testutils::Vec3Equals(child->GetRight(), glm::vec3(0.0f, 1.0f, 0.0f)));
    REQUIRE(testutils::Vec3Equals(child->GetFront(), glm::vec3(0.0f, 0.0f, -1.0f)));
}
