#include "skybox.frag.hpp"

namespace orc::shaders
{
    const char skybox_frag[152] = {
        '\x23','\x76','\x65','\x72','\x73','\x69','\x6f','\x6e','\x20','\x33','\x33','\x30','\x20','\x63','\x6f','\x72',
        '\x65','\x0a','\x0a','\x69','\x6e','\x20','\x76','\x65','\x63','\x33','\x20','\x63','\x6f','\x6f','\x72','\x64',
        '\x3b','\x0a','\x0a','\x75','\x6e','\x69','\x66','\x6f','\x72','\x6d','\x20','\x73','\x61','\x6d','\x70','\x6c',
        '\x65','\x72','\x43','\x75','\x62','\x65','\x20','\x63','\x75','\x62','\x65','\x6d','\x61','\x70','\x3b','\x0a',
        '\x0a','\x6f','\x75','\x74','\x20','\x76','\x65','\x63','\x34','\x20','\x63','\x6f','\x6c','\x6f','\x72','\x3b',
        '\x0a','\x0a','\x76','\x6f','\x69','\x64','\x20','\x6d','\x61','\x69','\x6e','\x28','\x29','\x0a','\x7b','\x0a',
        '\x20','\x20','\x20','\x20','\x63','\x6f','\x6c','\x6f','\x72','\x20','\x3d','\x20','\x76','\x65','\x63','\x34',
        '\x28','\x76','\x65','\x63','\x33','\x28','\x74','\x65','\x78','\x74','\x75','\x72','\x65','\x28','\x63','\x75',
        '\x62','\x65','\x6d','\x61','\x70','\x2c','\x20','\x63','\x6f','\x6f','\x72','\x64','\x29','\x29','\x2c','\x20',
        '\x31','\x2e','\x30','\x29','\x3b','\x0a','\x7d','\x0a'
    };
}