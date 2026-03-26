#include "task2.h"
#include "run.h"

#include <cmath>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>

#include "../task1/task1.h"

namespace tasks {
    namespace {
        int read_int(const char *prompt) {
            while (true) {
                std::cout << prompt;
                int value = 0;

                if (std::cin >> value) return value;

                std::cin.clear();
                std::cin.ignore(1000000, '\n');
                std::cout << "Некорректный ввод. Введите заново:" << std::endl;
            }
        }

        double read_probability(const char *prompt) {
            while (true) {
                std::cout << prompt;
                double probability = 0.0;

                if (std::cin >> probability) return probability;

                std::cin.clear();
                std::cin.ignore(1000000, '\n');
                std::cout << "Некорректный ввод. Введите заново:" << std::endl;
            }
        }

        std::int64_t abs_i64(std::int64_t value) {
            if (value >= 0) return value;
            return -value;
        }

        std::int64_t integer_power(std::int64_t base, int exponent) {
            std::int64_t result = 1;

            for (int i = 0; i < exponent; ++i) {
                if (result > (std::numeric_limits<std::int64_t>::max() / base)) {
                    throw std::overflow_error("Переполнение при возведении в степень.");
                }

                result *= base;
            }

            return result;
        }

        std::int64_t lower_bound_for_bits(int bits) {
            if (bits <= 1) return 2;
            return integer_power(2, bits - 1);
        }

        std::int64_t upper_bound_for_bits(int bits) {
            return integer_power(2, bits) - 1;
        }
    }

    bool rsa_key_generator::is_prime_candidate(std::int64_t value, double min_probability) const {
        miller_rabin_primality_test test;
        const auto result = test.test(value, min_probability);
        return result.is_probably_prime;
    }

    std::int64_t rsa_key_generator::generate_probable_prime(int bits, double min_probability) const {
        if (bits < 4 || bits > 31) {
            throw std::invalid_argument("Битовая длина простых чисел должна быть в диапазоне [4, 31].");
        }

        static std::mt19937_64 generator(std::random_device{}());
        const std::int64_t left = lower_bound_for_bits(bits);
        const std::int64_t right = upper_bound_for_bits(bits);
        std::uniform_int_distribution<std::int64_t> distribution(left, right);

        while (true) {
            std::int64_t candidate = distribution(generator);

            if ((candidate & 1LL) == 0) ++candidate;
            if (candidate > right) candidate -= 2;

            if (candidate < left) continue;
            if ((candidate & 1LL) == 0) continue;

            if (is_prime_candidate(candidate, min_probability)) return candidate;
        }
    }

    bool rsa_key_generator::pass_fermat_protection(std::int64_t p, std::int64_t q) const {
        const std::int64_t diff = abs_i64(p - q);
        const int p_bits = static_cast<int>(std::floor(std::log2(static_cast<long double>(p)))) + 1;
        const int threshold_power = std::max(1, p_bits / 2 - 1);
        const std::int64_t threshold = integer_power(2, threshold_power);

        return diff > threshold;
    }

    bool rsa_key_generator::pass_wiener_protection(std::int64_t n, std::int64_t d) const {
        const long double n_as_long_double = static_cast<long double>(n);
        const long double border = std::pow(n_as_long_double, 0.25L) / 3.0L;

        return static_cast<long double>(d) > border;
    }

    std::int64_t rsa_key_generator::choose_public_exponent(std::int64_t phi, std::int64_t n) const {
        const std::int64_t fixed_e = 65537;

        if (fixed_e < phi && gcd_euclid(fixed_e, phi) == 1) {
            const std::int64_t fixed_d = mod_inverse(fixed_e, phi);
            if (pass_wiener_protection(n, fixed_d)) return fixed_e;
        }

        static std::mt19937_64 generator(std::random_device{}());
        std::uniform_int_distribution<std::int64_t> distribution(3, phi - 2);

        while (true) {
            std::int64_t e = distribution(generator);

            if ((e & 1LL) == 0) ++e;
            if (e >= phi) e -= 2;
            if (e <= 1 || e >= phi) continue;
            if (gcd_euclid(e, phi) != 1) continue;

            const std::int64_t d = mod_inverse(e, phi);
            if (!pass_wiener_protection(n, d)) continue;

            return e;
        }
    }

    rsa_key_generator::key_pair rsa_key_generator::generate_keys(int prime_bits, double min_probability) const {
        if (prime_bits < 4 || prime_bits > 31) {
            throw std::invalid_argument("Битовая длина простых чисел должна быть в диапазоне [4, 31].");
        }

        if (min_probability < 0.5 || min_probability >= 1.0) {
            throw std::invalid_argument("Минимальная вероятность простоты должна быть в диапазоне [0.5, 1).");
        }

        while (true) {
            const std::int64_t p = generate_probable_prime(prime_bits, min_probability);
            std::int64_t q = generate_probable_prime(prime_bits, min_probability);

            while (q == p) q = generate_probable_prime(prime_bits, min_probability);
            if (!pass_fermat_protection(p, q)) continue;

            if (p > (std::numeric_limits<std::int64_t>::max() / q)) continue;
            const std::int64_t n = p * q;

            const std::int64_t p_minus_one = p - 1;
            const std::int64_t q_minus_one = q - 1;

            if (p_minus_one > (std::numeric_limits<std::int64_t>::max() / q_minus_one)) continue;
            const std::int64_t phi = p_minus_one * q_minus_one;

            const std::int64_t e = choose_public_exponent(phi, n);
            const std::int64_t d = mod_inverse(e, phi);

            if (!pass_wiener_protection(n, d)) continue;

            return {
                {n, e},
                {n, d},
                p,
                q,
                phi
            };
        }
    }

    void task2::run(int argc, char **argv) {
        rsa_key_generator generator;

        try {
            const int prime_bits = read_int("Введите битовую длину простых чисел [4, 31]: ");
            const double min_probability =
                read_probability("Введите минимальную вероятность простоты [0.5, 1): ");

            const auto keys = generator.generate_keys(prime_bits, min_probability);

            std::cout << std::endl << "Ключи RSA успешно сгенерированы:" << std::endl;
            std::cout << "p = " << keys.p << std::endl;
            std::cout << "q = " << keys.q << std::endl;
            std::cout << "n = p * q = " << keys.open.n << std::endl;
            std::cout << "phi(n) = (p - 1) * (q - 1) = " << keys.phi << std::endl;
            std::cout << "Открытый ключ: (e = " << keys.open.e << ", n = " << keys.open.n << ")" << std::endl;
            std::cout << "Закрытый ключ: (d = " << keys.close.d << ", n = " << keys.close.n << ")" << std::endl;
            std::cout << "Защита от атаки Ферма обеспечена (p и q не близки)." << std::endl;
            std::cout << "Защита от атаки Винера обеспечена (d > n^(1/4)/3)." << std::endl;
        } catch (const std::exception &e) {
            std::cout << "Ошибка: " << e.what() << std::endl;
        }
    }
}
