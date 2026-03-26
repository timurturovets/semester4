#include "task1.h"
#include "run.h"

#include <cmath>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../hw5/task1/number_theory_service.cpp"

namespace tasks {
    namespace {
        int read_choice() {
            while (true) {
                std::cout << std::endl << "Выберите действие: ";
                int choice = -1;

                if (std::cin >> choice) return choice;

                std::cin.clear();
                std::cin.ignore(1000000, '\n');
                std::cout << "Некорректный ввод. Введите заново:" << std::endl;
            }
        }

        std::int64_t read_int64(const char *prompt) {
            while (true) {
                std::cout << prompt;
                std::int64_t value = 0;

                if (std::cin >> value) return value;

                std::cin.clear();
                std::cin.ignore(1000000, '\n');
                std::cout << "Некорректный ввод. Введите заново:" << std::endl;
            }
        }

        double read_probability(const char *prompt) {
            while (true) {
                std::cout << prompt;
                double value = 0.0;

                if (std::cin >> value) return value;

                std::cin.clear();
                std::cin.ignore(1000000, '\n');
                std::cout << "Некорректный ввод. Введите заново:" << std::endl;
            }
        }

        int calculate_iterations(double min_probability, double witness_probability) {
            if (min_probability < 0.5 || min_probability >= 1.0) {
                throw std::invalid_argument("Минимальная вероятность должна быть в диапазоне [0.5, 1).");
            }

            if (witness_probability <= 0.0 || witness_probability >= 1.0) {
                throw std::invalid_argument("Вероятность обнаружения должна быть в диапазоне (0, 1).");
            }

            if (min_probability <= witness_probability) return 1;

            const double failure_probability = 1.0 - min_probability;
            const double one_iteration_failure = 1.0 - witness_probability;
            const int iterations = static_cast<int>(
                std::ceil(std::log(failure_probability) / std::log(one_iteration_failure))
            );

            if (iterations < 1) return 1;
            return iterations;
        }

        void print_result(std::int64_t value,
                          const probabilistic_primality_test &test,
                          const probabilistic_primality_test::result &result) {
            std::cout << "[" << test.name() << "] n = " << value << '\n';
            std::cout << "Число итераций: " << result.iterations_used << '\n';
            std::cout << "Достигнутая вероятность: " << result.achieved_probability << '\n';
            std::cout << (result.is_probably_prime ? "Число вероятно простое.\n" : "Число составное.\n");
        }

        void run_for_value(std::int64_t value,
                           double min_probability,
                           const solovay_strassen_primality_test &solovay_strassen,
                           const miller_rabin_primality_test &miller_rabin) {
            const auto ss_result = solovay_strassen.test(value, min_probability);
            const auto mr_result = miller_rabin.test(value, min_probability);

            print_result(value, solovay_strassen, ss_result);
            print_result(value, miller_rabin, mr_result);
        }
    }

    probabilistic_primality_test::result probabilistic_primality_test::test(
        std::int64_t value, double min_probability
    ) const {
        if (min_probability < 0.5 || min_probability >= 1.0) {
            throw std::invalid_argument("Минимальная вероятность должна быть в диапазоне [0.5, 1).");
        }

        if (value < 2) return {false, 0.0, 0};
        if (value == 2 || value == 3) return {true, 1.0, 0};
        if (value % 2 == 0) return {false, 1.0, 0};

        const double witness_probability = composite_witness_probability();
        const int iterations = calculate_iterations(min_probability, witness_probability);

        for (int i = 0; i < iterations; ++i) {
            const std::int64_t witness = choose_witness(value);
            const bool passed = run_iteration(value, witness);
            const double probability = 1.0 - std::pow(1.0 - witness_probability, i + 1);

            if (!passed) return {false, probability, i + 1};
        }

        const double achieved_probability = 1.0 - std::pow(1.0 - witness_probability, iterations);
        return {true, achieved_probability, iterations};
    }

    std::int64_t probabilistic_primality_test::choose_witness(std::int64_t value) const {
        static std::mt19937_64 generator(std::random_device{}());
        std::uniform_int_distribution<std::int64_t> distribution(2, value - 2);
        return distribution(generator);
    }

    const char *solovay_strassen_primality_test::name() const {
        return "Соловея-Штрассена";
    }

    bool solovay_strassen_primality_test::run_iteration(std::int64_t value, std::int64_t witness) const {
        if (gcd_euclid(witness, value) != 1) return false;

        const int jacobi = jacobi_symbol(witness, value);
        if (jacobi == 0) return false;

        const std::int64_t lhs = pow_mod(witness, (value - 1) / 2, value);
        const std::int64_t rhs = normalize_mod(static_cast<std::int64_t>(jacobi), value);

        return lhs == rhs;
    }

    double solovay_strassen_primality_test::composite_witness_probability() const {
        return 0.5;
    }

    const char *miller_rabin_primality_test::name() const {
        return "Миллера-Рабина";
    }

    bool miller_rabin_primality_test::run_iteration(std::int64_t value, std::int64_t witness) const {
        if (gcd_euclid(witness, value) != 1) return false;

        std::int64_t d = value - 1;
        int s = 0;
        while ((d & 1LL) == 0) {
            d >>= 1LL;
            ++s;
        }

        std::int64_t x = pow_mod(witness, d, value);
        if (x == 1 || x == value - 1) return true;

        for (int r = 1; r < s; ++r) {
            x = pow_mod(x, 2, value);
            if (x == value - 1) return true;
            if (x == 1) return false;
        }

        return false;
    }

    double miller_rabin_primality_test::composite_witness_probability() const {
        return 0.75;
    }

    void task1::run(int argc, char **argv) {
        const solovay_strassen_primality_test solovay_strassen;
        const miller_rabin_primality_test miller_rabin;

        while (true) {
            std::cout << "1. Проверить одно число" << std::endl;
            std::cout << "2. Демонстрация на наборе чисел" << std::endl;
            std::cout << "0. Выход" << std::endl;

            const int choice = read_choice();
            if (choice == 0) return;

            try {
                if (choice == 1) {
                    const std::int64_t value = read_int64("Введите n: ");

                    const double min_probability = read_probability(
                        "Введите минимальную вероятность [0.5, 1): "
                    );

                    run_for_value(value, min_probability, solovay_strassen, miller_rabin);
                    continue;
                }

                if (choice == 2) {
                    const double min_probability = read_probability(
                        "Введите минимальную вероятность для демонстрации [0.5, 1): "
                    );

                    const std::vector<std::int64_t> values = {
                        17, 19, 97, 561, 1105, 1729, 2465, 6601, 8911
                    };

                    for (const std::int64_t value: values) {
                        std::cout << std::endl << "n = " << value << std::endl;
                        run_for_value(value, min_probability, solovay_strassen, miller_rabin);
                    }
                }
            } catch (const std::exception &e) {
                std::cout << "Ошибка: " << e.what() << std::endl;
            }
        }
    }
}
