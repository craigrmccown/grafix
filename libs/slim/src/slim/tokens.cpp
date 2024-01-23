#include <string>
#include <vector>
#include "tokens.hpp"

namespace slim
{
    const std::vector<std::string> patterns
    {
        "property",
        "shared",
        "feature",
        "shader",
        "return",
        "=",
        "\\|\\|",
        "&&",
        "==",
        "!=",
        ">",
        "<",
        ">=",
        "<=",
        "\\+",
        "-",
        "\\*",
        "/",
        "%",
        "!",
        "\\(",
        "\\)",
        "\\[",
        "\\]",
        "{",
        "}",
        "\\.",
        ",",
        ";",
        "(vertex|fragment)",
        "(bool|int|uint|float|bvec2|bvec3|bvec4|ivec2|ivec3|ivec4|uvec2|uvec3|uvec4|vec2|vec3|vec4|sampler2D)",
        "(true|false)",
        "[0-9]+(\\.[0-9]+)?",
        "\".*\"",
        "[a-zA-Z_][a-zA-Z0-9_]*",
        "#[a-z_]+"
    };
}
