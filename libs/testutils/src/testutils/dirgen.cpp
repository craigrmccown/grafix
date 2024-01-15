#include <catch2/generators/catch_generators_range.hpp>
#include <filesystem>
#include <string>
#include "dirgen.hpp"

namespace testutils
{
    // A Catch2 generator that enumerates the contents of a directory at test
    // runtime. All paths, including both the input and output paths are relative
    // to the current working directory.
    //
    // TODO: Handle file paths in a less error-prone way (e.g. a combination of
    // absolute paths and an environment variable for portability)
    Catch::Generators::GeneratorWrapper<std::filesystem::directory_entry> dir(const std::string &path)
    {
        std::filesystem::directory_iterator it(path);
        return Catch::Generators::GeneratorWrapper<std::filesystem::directory_entry>(
            new Catch::Generators::IteratorGenerator<std::filesystem::directory_entry>(
                std::filesystem::begin(it),
                std::filesystem::end(it)
            )
        );
    }
}
