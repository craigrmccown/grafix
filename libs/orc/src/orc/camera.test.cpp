#include <memory>
#include <catch2/catch_test_macros.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <testutils/glm.hpp>
#include "camera.hpp"

TEST_CASE("View-projection matrix", "[orc]") {
    float fov = 45.0f, aspect = 16.0f/9.0f;
    std::shared_ptr<orc::Camera> camera = orc::Camera::Create();

    // Initially, we should be looking in the -Z direction at the origin. The
    // view matrix is constructed as follows:
    //
    // [ Rx, Ry, Rz, 0 ]   [ 1  0  0 -Tx ]
    // [ Ux, Uy, Uz, 0 ] * [ 0  1  0 -Ty ]
    // [ Fx, Fy, Fz, 0 ]   [ 0  0  1 -Tz ]
    // [ 0,  0,  0,  1 ]   [ 0  0  0  1  ]
    //
    // Where R, U, F, and T are the right, up, front, and translation vectors,
    // respectively. The first matrix represents a rotation, and the second a
    // translation.
    glm::mat4 projectionMx = glm::perspective(glm::radians(fov), aspect, 0.1f, 100.0f);
    glm::mat4 rotationMx(1.0f);
    glm::mat4 translationMx(1.0f);
    REQUIRE(testutils::Mat4Equals(projectionMx * rotationMx * translationMx, camera->GetViewProjectionMx()));

    // Back the camera up a bit
    camera->Translate(0.0f, 0.0f, 10.0f);
    camera->ComputeMxs();
    translationMx[3][2] = -10.0f;
    REQUIRE(testutils::Mat4Equals(projectionMx * rotationMx * translationMx, camera->GetViewProjectionMx()));

    // Raise the camera into the air by the same distance, then angle it down
    // towards the origin
    camera->Translate(0.0f, 10.0f, 0.0f);
    camera->Rotate(0.0f, glm::radians(-45.0f), 0.0f);
    camera->ComputeMxs();
    translationMx[3][1] = -10.0f;

    // Because we are viewing at a 45 degree angle, we can pre-compute the unit
    // vector components using the pythagorean theorem
    double vecComp = sqrt(0.5);
    rotationMx[1][1] = vecComp;    // Uy
    rotationMx[2][1] = -vecComp;    // Uz
    rotationMx[1][2] = vecComp;   // Fy
    rotationMx[2][2] = vecComp;    // Fz

    REQUIRE(testutils::Mat4Equals(projectionMx * rotationMx * translationMx, camera->GetViewProjectionMx()));
}
