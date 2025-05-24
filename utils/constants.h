#pragma once
#include <cstdint>

enum class FileMode : uint32_t {
    Blob = 0100644,
    Exec = 0100755,
    Tree = 0040000,
    Commit = 0000105,
    Path = 000644
};

