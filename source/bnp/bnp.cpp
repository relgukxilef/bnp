#include <bnp/bnp.h>

using namespace bnp;
using namespace std;

uint8_t byte(view &bytes) {
    uint8_t value = *bytes.begin;
    bytes.begin = min(bytes.begin + 1, bytes.end);
    return value;
}

void byte(uint8_t value, view &bytes) {
    *bytes.begin = value;
    bytes.begin = min(bytes.begin + 1, bytes.end);
}

unsigned leb128(view &bytes) {
    auto sum = 0u;
    auto shift = 0u;
    do {
        auto value = byte(bytes);
        sum += (value & 0x7f) << shift;
        shift += 7;
    } while (sum >= 128);
    return sum;
}

void leb128(unsigned value, view &bytes) {
    do {
        auto next = value & 0x7f;
        value >>= 7;
        if (value > 0)
            next |= 0x80;
        byte(next, bytes);
    } while (value > 0);
}
