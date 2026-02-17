#pragma once

#include <cstdint>
#include <functional>
#include <unordered_map>
#include <vector>
namespace tasks {
    class bit_substitutor {
    public:
        static std::vector<uint8_t> substitute(
            std::vector<uint8_t> const &data,
            size_t block_size,
            std::unordered_map<size_t, size_t> const &sBlock
        );

        static std::vector<uint8_t> substitute(
            std::vector<uint8_t> const &data,
            size_t block_size,
            std::function<size_t(size_t)> const &func
        );

        static std::unordered_map<size_t, size_t> read_sBlock(size_t n);
    private:
        static std::vector<uint8_t> substitute_inner(
            std::vector<uint8_t> const &data,
            size_t block_size,
            std::function<size_t(size_t)> const &func
        );

    };
}