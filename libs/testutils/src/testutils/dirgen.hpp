#include <catch2/generators/catch_generators.hpp>
#include <filesystem>
#include <string>

namespace testutils
{
    Catch::Generators::GeneratorWrapper<std::filesystem::directory_entry> dir(const std::string &path);
}
