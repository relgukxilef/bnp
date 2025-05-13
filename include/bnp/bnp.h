#pragma once

#include <cstdint>
#include <vector>

namespace bnp {
    struct id {
        std::uint8_t data[16];
    };

    struct field {
        id id;
        enum {
            fixed, dynamic
        } primitive_type;
        unsigned dynamic_size;
    };

    struct view {
        std::uint8_t *begin, *end;
    };

    unsigned leb128(view bytes);
    void leb128(unsigned value, view &bytes);
}
