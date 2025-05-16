#pragma once

#include <cstdint>
#include <vector>
#include <memory>

namespace bnp {
    struct id {
        std::uint8_t data[16];
    };

    struct field {
        id id;
        enum {
            fixed, dynamic
        } primitive_type;
        unsigned fixed_size;
    };

    struct schema {
        std::initializer_list<field> fields;
    };

    struct connection {
        std::unique_ptr<unsigned[]> field_indices;
    };

    struct view {
        std::uint8_t *data;
        std::size_t size;
    };

    struct const_view {
        const std::uint8_t *data;
        std::size_t size;
    };

    unsigned leb128(const_view &bytes);
    void leb128(unsigned value, view &bytes);

    void initial_server_message(const schema server_schema, view &bytes);
    connection initial_server_message(
        const schema client_schema, const_view &bytes
    );
    void initial_client_message(
        const connection &c, const schema client_schema, view &bytes
    );
    connection initial_client_message(
        const schema server_schema, const_view &bytes
    );
}
