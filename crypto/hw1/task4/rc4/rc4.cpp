#include <stdexcept>

#include "rc4.h"

namespace tasks {
    RC4::RC4(std::vector<uint8_t> const &key) {
        if (key.empty()) throw std::invalid_argument("Ключ не может быть пустым");

        ksa(key);
    }

    void RC4::ksa(std::vector<uint8_t> const &key) {
        for (int idx = 0; idx < 256; ++idx) {
            S[idx] = static_cast<uint8_t>(idx);
        }

        uint8_t j_local = 0;

        for (int idx = 0; idx < 256; ++idx) {
            j_local = j_local + S[idx] + key[idx % key.size()];
            std::swap(S[idx], S[j_local]);
        }

        i = 0;
        j = 0;
    }

    uint8_t RC4::prga() {
        i++;
        j += S[i];

        std::swap(S[i], S[j]);

        return S[(S[i] + S[j])];
    }

    void RC4::process(std::vector<uint8_t> &data) {
        for (auto &byte : data) {
            byte ^= prga();
        }
    }
}
