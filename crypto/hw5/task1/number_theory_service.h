#pragma once

#include <cstdint>

namespace tasks {
    class number_theory_service {
    public:
        struct extended_gcd_result {
            std::int64_t gcd;
            std::int64_t x;
            std::int64_t y;
        };

        static std::int64_t gcd_euclid(std::int64_t a, std::int64_t b);
        static extended_gcd_result gcd_euclid_extended(std::int64_t a, std::int64_t b);
        static std::int64_t pow_mod(std::int64_t base, std::int64_t exponent, std::int64_t mod);
        static std::int64_t mod_inverse(std::int64_t value, std::int64_t n);
        static std::int64_t euler_phi_definition(std::int64_t n);
        static std::int64_t euler_phi_factorization(std::int64_t n);
        static std::int64_t euler_phi_fourier(std::int64_t n);
        static int legendre_symbol(std::int64_t a, std::int64_t p);
        static int jacobi_symbol(std::int64_t a, std::int64_t n);

    protected:
        static std::int64_t normalize_mod(std::int64_t value, std::int64_t mod);
        static std::int64_t multiply_mod(std::int64_t left, std::int64_t right, std::int64_t mod);
        static std::int64_t mod_pow(std::int64_t base, std::int64_t exponent, std::int64_t mod);
        static bool is_odd_prime(std::int64_t value);
    };
}
