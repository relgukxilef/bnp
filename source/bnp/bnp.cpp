#include <bnp/bnp.h>

#include <array>

#include "../test/test.h"

using namespace std;

namespace bnp {
    uint8_t byte(const_view &bytes) {
        uint8_t value = *bytes.data;
        bytes.data = min(bytes.data + 1, bytes.data + bytes.size);
        bytes.size = max<size_t>(bytes.size - 1, 1);
        return value;
    }

    void byte(uint8_t value, view &bytes) {
        *bytes.data = value;
        bytes.data = min(bytes.data + 1, bytes.data + bytes.size);
        bytes.size = max<size_t>(bytes.size - 1, 1);
    }

    unsigned leb128(const_view &bytes) {
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

    void initial_server_message(const schema server_schema, view &bytes) {
        byte(0, bytes);
        byte(0, bytes);
        leb128(server_schema.fields.size(), bytes);
        for (auto &field : server_schema.fields) {

        }
    }

    connection initial_server_message(
        const schema client_schema, const_view &bytes
    );

    void initial_client_message(
        const connection &c, const schema client_schema, view &bytes
    );

    connection initial_client_message(
        const schema server_schema, const_view &bytes
    );
    
 
    test leb128_test([](){
        array<uint8_t, 4> buffer;
        view buffer_view{data(buffer), size(buffer)};
        auto bytes = buffer_view;
        leb128(42, bytes);
        const_view buffer_const_view{data(buffer), size(buffer)};
        checked(42) == leb128(buffer_const_view);
        checked(buffer[0]) == 42;
        checked(bytes.size) == 3;
        checked(buffer_const_view.size) == 3;
    });
}
