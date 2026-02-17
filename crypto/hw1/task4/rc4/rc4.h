#pragma once

#include <cstdint>
#include <vector>

namespace tasks {
    class RC4 {
    public:
        explicit RC4(std::vector<uint8_t> const &key);

        void process(std::vector<uint8_t> &data);

    private:
        uint8_t S[256]{};
        uint8_t i = 0;
        uint8_t j = 0;

        void ksa(std::vector<uint8_t> const &key);
        uint8_t prga();
    };
}
