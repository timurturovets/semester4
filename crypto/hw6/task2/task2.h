#pragma once

#include <cstdint>

#include "../../hw5/task1/number_theory_service.h"

namespace tasks {
    class rsa_key_generator final : private number_theory_service {
    public:
        struct public_key {
            std::int64_t n;
            std::int64_t e;
        };

        struct private_key {
            std::int64_t n;
            std::int64_t d;
        };

        struct key_pair {
            public_key open;
            private_key close;
            std::int64_t p;
            std::int64_t q;
            std::int64_t phi;
        };

        [[nodiscard]] key_pair generate_keys(int prime_bits, double min_probability) const;

    private:
        [[nodiscard]] std::int64_t generate_probable_prime(int bits, double min_probability) const;
        [[nodiscard]] bool is_prime_candidate(std::int64_t value, double min_probability) const;
        [[nodiscard]] bool pass_fermat_protection(std::int64_t p, std::int64_t q) const;
        [[nodiscard]] bool pass_wiener_protection(std::int64_t n, std::int64_t d) const;
        [[nodiscard]] std::int64_t choose_public_exponent(std::int64_t phi, std::int64_t n) const;
    };
}
