#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <catch2/catch_test_macros.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/epsilon.hpp>
#include "camera.hpp"

std::string vec3ToString(glm::vec3 v)
{
    std::stringstream out;

    out
        << std::fixed << std::setprecision(4)
        << v.x << ", " << v.y << ", " << v.z;

    return out.str();
}

std::string mat4ToString(glm::mat4 m)
{
    std::stringstream out;
    out << std::fixed << std::setprecision(10);

    for (int r = 0; r < 4; r ++)
    {
        if (r != 0) out << "\n";

        for (int c = 0; c < 4; c ++)
        {
            if (c != 0) out << ", ";
            out << m[c][r];
        }
    }

    return out.str();
}

void printMat4Err(glm::mat4 expected, glm::mat4 actual)
{
    std::cout
        << "Matrices are not equal\n"
        << "Expected:\n"
        << mat4ToString(expected) << "\n\n"
        << "Actual:\n"
        << mat4ToString(actual) << std::endl;
}

bool compareMat4(glm::mat4 expected, glm::mat4 actual)
{
    for (int c = 0; c < 4; c ++)
    {
        for (int r = 0; r < 4; r ++)
        {
            float diff = abs(expected[c][r] - actual[c][r]);
            float epsilon = std::numeric_limits<float>::epsilon();

            if (diff > epsilon * 100)
            {
                printMat4Err(expected, actual);
                return false;
            }
        }
    }
    return true;
}

TEST_CASE("View-projection matrix", "[orc]") {
    float fov = 45.0f, aspect = 16.0f/9.0f;
    orc::Camera camera(fov, aspect);

    // Initially, we should be looking in the +Z direction at the origin. The
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
    glm::mat4 projectionMx = glm::perspective(glm::radians(fov), aspect, 0.0f, 100.0f);
    glm::mat4 rotationMx(1.0f);
    glm::mat4 translationMx(1.0f);
    REQUIRE(compareMat4(projectionMx * rotationMx * translationMx, camera.GetViewProjectionMx()));

    // Back the camera up a bit
    camera.Translate(0.0f, 0.0f, -10.0f);
    camera.ComputeMxs();
    translationMx[3][2] = 10.0f;
    REQUIRE(compareMat4(projectionMx * rotationMx * translationMx, camera.GetViewProjectionMx()));

    // Raise the camera into the air by the same distance, then angle it down
    // towards the origin
    camera.Translate(0.0f, 10.0f, 0.0f);
    camera.Rotate(0.0f, glm::radians(45.0), 0.0f);
    camera.ComputeMxs();
    translationMx[3][1] = -10.0f;

    // Because we are viewing at a 45 degree angle, we can pre-compute the unit
    // vector components using the pythagorean theorem
    double vecComp = sqrt(0.5);
    rotationMx[1][1] = vecComp;    // Uy
    rotationMx[2][1] = vecComp;    // Uz
    rotationMx[1][2] = -vecComp;   // Fy
    rotationMx[2][2] = vecComp;    // Fz

    REQUIRE(compareMat4(projectionMx * rotationMx * translationMx, camera.GetViewProjectionMx()));
}
