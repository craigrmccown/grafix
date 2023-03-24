#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <glm/gtc/epsilon.hpp>
#include <glm/glm.hpp>
#include "glm.hpp"

const float errorMargin = std::numeric_limits<float>::epsilon() * 100;

namespace testutils
{
    static std::string vec3ToString(glm::vec3 v)
    {
        std::stringstream out;

        out
            << std::fixed << std::setprecision(4)
            << v.x << ", " << v.y << ", " << v.z;

        return out.str();
    }

    static void printVec3Error(glm::vec3 expected, glm::vec3 actual)
    {
        std::cout
            << "Vectors are not equal\n"
            << "Expected:\n"
            << vec3ToString(expected) << "\n\n"
            << "Actual:\n"
            << vec3ToString(actual) << std::endl;
    }

    bool Vec3Equals(glm::vec3 expected, glm::vec3 actual) {
        bool eq = glm::all(glm::epsilonEqual(expected, actual, errorMargin));
        if (!eq) printVec3Error(expected, actual);
        return eq;
    }

    static std::string mat4ToString(glm::mat4 m)
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

    static void printMat4Err(glm::mat4 expected, glm::mat4 actual)
    {
        std::cout
            << "Matrices are not equal\n"
            << "Expected:\n"
            << mat4ToString(expected) << "\n\n"
            << "Actual:\n"
            << mat4ToString(actual) << std::endl;
    }

    bool Mat4Equals(glm::mat4 expected, glm::mat4 actual)
    {
        for (int c = 0; c < 4; c ++)
        {
            for (int r = 0; r < 4; r ++)
            {
                float diff = abs(expected[c][r] - actual[c][r]);

                if (diff > errorMargin)
                {
                    printMat4Err(expected, actual);
                    return false;
                }
            }
        }
        return true;
    }
}
