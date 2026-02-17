#pragma once

#include <cstdint>

namespace tasks {
    class bit_utils {
    public:
        static void print_bits(uint64_t value, size_t n);

        static uint64_t rotate_left(uint64_t value, size_t n, size_t k);

        static uint64_t rotate_right(uint64_t value, size_t n, size_t k);

        static uint64_t apply_mask(uint64_t value, size_t n, size_t k);

        static uint64_t get_bits(uint64_t value, size_t n, size_t i, size_t j);

        static uint64_t swap_bits(uint64_t value, size_t n, size_t i, size_t j);

        static uint64_t set_bit(uint64_t value, size_t n, size_t i, bool bit_value);
    };
}
