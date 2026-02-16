#pragma once

#include <cstdint>
#include <vector>

namespace tasks {
    enum class bit_order {
        LSB_FIRST,
        MSB_FIRST
    };

    enum class index_base {
        ZERO_BASED,
        ONE_BASED
    };

    class bit_permutator {
    public:
        static std::vector<uint8_t> permute(
            std::vector<uint8_t> const &data,
            std::vector<size_t> const &pBlock,
            bit_order order,
            index_base base
        );

    private:
        static bool getBit(std::vector<uint8_t> const &data, size_t bit_index, bit_order order);
        static void setBit(std::vector<uint8_t> &data, size_t bit_index, bool value, bit_order order);

    };
}