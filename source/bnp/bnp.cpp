#include <bnp/bnp.h>

#include "../test/test.h"

#include <array>

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
        auto value = 0u;
        do {
            value = byte(bytes);
            sum += (value & 0x7f) << shift;
            shift += 7;
        } while (value >= 128);
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


    template<class T, std::size_t N>
    constexpr std::size_t size(const T (&array)[N]) noexcept {
        return N;
    }

    test leb128_test([](){
        unsigned examples[] = {
            0,
            42,
            127,
            128,
            192,
            16384,
            268435455,
        };
        uint8_t buffer[4];
        
        for (int i = 0; i < size(examples); i++) {
            auto value = examples[i];
            view buffer_view{buffer, size(buffer)};
            auto bytes = buffer_view;
            leb128(value, bytes);
            const_view buffer_const_view{buffer, size(buffer)};
            checked(value) == leb128(buffer_const_view);
            checked(bytes.size) == buffer_const_view.size;
        }
    });
}
