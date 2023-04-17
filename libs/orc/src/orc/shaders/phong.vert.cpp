#include "phong.vert.hpp"

namespace orc::shaders
{
    const char phong_vert[616] = {
        '\x23','\x76','\x65','\x72','\x73','\x69','\x6f','\x6e','\x20','\x33','\x33','\x30','\x20','\x63','\x6f','\x72',
        '\x65','\x0a','\x0a','\x6c','\x61','\x79','\x6f','\x75','\x74','\x20','\x28','\x6c','\x6f','\x63','\x61','\x74',
        '\x69','\x6f','\x6e','\x20','\x3d','\x20','\x30','\x29','\x20','\x69','\x6e','\x20','\x76','\x65','\x63','\x33',
        '\x20','\x76','\x61','\x5f','\x63','\x6f','\x6f','\x72','\x64','\x73','\x3b','\x0a','\x6c','\x61','\x79','\x6f',
        '\x75','\x74','\x20','\x28','\x6c','\x6f','\x63','\x61','\x74','\x69','\x6f','\x6e','\x20','\x3d','\x20','\x31',
        '\x29','\x20','\x69','\x6e','\x20','\x76','\x65','\x63','\x33','\x20','\x76','\x61','\x5f','\x6e','\x6f','\x72',
        '\x6d','\x61','\x6c','\x3b','\x0a','\x6c','\x61','\x79','\x6f','\x75','\x74','\x20','\x28','\x6c','\x6f','\x63',
        '\x61','\x74','\x69','\x6f','\x6e','\x20','\x3d','\x20','\x32','\x29','\x20','\x69','\x6e','\x20','\x76','\x65',
        '\x63','\x32','\x20','\x76','\x61','\x5f','\x74','\x65','\x78','\x43','\x6f','\x6f','\x72','\x64','\x73','\x3b',
        '\x0a','\x0a','\x6f','\x75','\x74','\x20','\x76','\x65','\x63','\x32','\x20','\x76','\x73','\x5f','\x6f','\x75',
        '\x74','\x5f','\x74','\x65','\x78','\x43','\x6f','\x6f','\x72','\x64','\x73','\x3b','\x0a','\x6f','\x75','\x74',
        '\x20','\x76','\x65','\x63','\x33','\x20','\x76','\x73','\x5f','\x6f','\x75','\x74','\x5f','\x6e','\x6f','\x72',
        '\x6d','\x61','\x6c','\x3b','\x0a','\x6f','\x75','\x74','\x20','\x76','\x65','\x63','\x33','\x20','\x76','\x73',
        '\x5f','\x6f','\x75','\x74','\x5f','\x66','\x72','\x61','\x67','\x50','\x6f','\x73','\x3b','\x0a','\x0a','\x75',
        '\x6e','\x69','\x66','\x6f','\x72','\x6d','\x20','\x6d','\x61','\x74','\x34','\x20','\x75','\x5f','\x74','\x72',
        '\x61','\x6e','\x73','\x66','\x6f','\x72','\x6d','\x4d','\x78','\x3b','\x0a','\x75','\x6e','\x69','\x66','\x6f',
        '\x72','\x6d','\x20','\x6d','\x61','\x74','\x34','\x20','\x75','\x5f','\x6d','\x6f','\x64','\x65','\x6c','\x4d',
        '\x78','\x3b','\x0a','\x0a','\x76','\x6f','\x69','\x64','\x20','\x6d','\x61','\x69','\x6e','\x28','\x29','\x0a',
        '\x7b','\x0a','\x20','\x20','\x20','\x20','\x67','\x6c','\x5f','\x50','\x6f','\x73','\x69','\x74','\x69','\x6f',
        '\x6e','\x20','\x3d','\x20','\x75','\x5f','\x74','\x72','\x61','\x6e','\x73','\x66','\x6f','\x72','\x6d','\x4d',
        '\x78','\x20','\x2a','\x20','\x76','\x65','\x63','\x34','\x28','\x76','\x61','\x5f','\x63','\x6f','\x6f','\x72',
        '\x64','\x73','\x2c','\x20','\x31','\x2e','\x30','\x29','\x3b','\x0a','\x20','\x20','\x20','\x20','\x76','\x73',
        '\x5f','\x6f','\x75','\x74','\x5f','\x74','\x65','\x78','\x43','\x6f','\x6f','\x72','\x64','\x73','\x20','\x3d',
        '\x20','\x76','\x61','\x5f','\x74','\x65','\x78','\x43','\x6f','\x6f','\x72','\x64','\x73','\x3b','\x0a','\x0a',
        '\x20','\x20','\x20','\x20','\x2f','\x2f','\x20','\x43','\x6f','\x6d','\x70','\x75','\x74','\x65','\x20','\x66',
        '\x72','\x61','\x67','\x6d','\x65','\x6e','\x74','\x20','\x70','\x6f','\x73','\x69','\x74','\x69','\x6f','\x6e',
        '\x20','\x61','\x6e','\x64','\x20','\x6e','\x6f','\x72','\x6d','\x61','\x6c','\x20','\x64','\x69','\x72','\x65',
        '\x63','\x74','\x69','\x6f','\x6e','\x20','\x69','\x6e','\x20','\x77','\x6f','\x72','\x6c','\x64','\x20','\x73',
        '\x70','\x61','\x63','\x65','\x20','\x62','\x79','\x20','\x61','\x70','\x70','\x6c','\x79','\x69','\x6e','\x67',
        '\x0a','\x20','\x20','\x20','\x20','\x2f','\x2f','\x20','\x6d','\x6f','\x64','\x65','\x6c','\x20','\x74','\x72',
        '\x61','\x6e','\x73','\x66','\x6f','\x72','\x6d','\x61','\x74','\x69','\x6f','\x6e','\x0a','\x20','\x20','\x20',
        '\x20','\x76','\x73','\x5f','\x6f','\x75','\x74','\x5f','\x6e','\x6f','\x72','\x6d','\x61','\x6c','\x20','\x3d',
        '\x20','\x76','\x65','\x63','\x33','\x28','\x75','\x5f','\x6d','\x6f','\x64','\x65','\x6c','\x4d','\x78','\x20',
        '\x2a','\x20','\x76','\x65','\x63','\x34','\x28','\x76','\x61','\x5f','\x6e','\x6f','\x72','\x6d','\x61','\x6c',
        '\x2c','\x20','\x30','\x2e','\x30','\x29','\x29','\x3b','\x0a','\x20','\x20','\x20','\x20','\x76','\x73','\x5f',
        '\x6f','\x75','\x74','\x5f','\x66','\x72','\x61','\x67','\x50','\x6f','\x73','\x20','\x3d','\x20','\x76','\x65',
        '\x63','\x33','\x28','\x75','\x5f','\x6d','\x6f','\x64','\x65','\x6c','\x4d','\x78','\x20','\x2a','\x20','\x76',
        '\x65','\x63','\x34','\x28','\x76','\x61','\x5f','\x63','\x6f','\x6f','\x72','\x64','\x73','\x2c','\x20','\x31',
        '\x2e','\x30','\x29','\x29','\x3b','\x0a','\x7d','\x0a'
    };
}
