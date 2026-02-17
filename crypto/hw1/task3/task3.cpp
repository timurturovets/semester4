#include <stdexcept>
#include <iostream>

#include "run.h"
#include "task3.h"

// ReSharper disable file CppLocalVariableMayBeConst
// ReSharper disable file CppParameterMayBeConst
namespace tasks {
    void task3::run(int argc, char **argv) {
        uint64_t value;
        size_t n, k;

        do {
            std::cout << "Введите количество бит (от 1 до 64): ";
            std::cin >> n;
        } while (n == 0 || n > 64);

        uint64_t max_value = n == 64 ? ~0ULL : (1ULL << n) - 1;

        do {
            std::cout << "Введите " << n << "-битовое значение (0.. " << max_value << "): ";
            std::cin >> value;

        } while (value > max_value);

        int choice = -1;
        std::cout << "1. Побитовый циклический сдвиг n-битового значения на k битов влево" << std::endl;
        std::cout << "2. Побитовый циклический сдвиг n-битового значения на k битов вправо" << std::endl;
        std::cout << "3. Применение k-битовой маски к n-битовому значению" << std::endl;
        std::cout << "4. Получить биты с i по j" << std::endl;
        std::cout << "5. Поменять местами i-ый и j-ый биты" << std::endl;
        std::cout << "6. Установить 0/1 i-го бита" << std::endl;
        std::cout << "0. Выход" << std::endl;

        do {
            std::cout << std::endl << "Текущее значение: ";
            bit_utils::print_bits(value, n);

            std::cout << std::endl << "Выберите задачу: ";

            std::cin >> choice;
            switch (choice) {
                case 0:
                    return;

                case 1: {
                    do {
                        std::cout << "Введите количество бит для сдвига k: (0.." << n << "): ";
                        std::cin >> k;

                    } while (k > n);

                    value = bit_utils::rotate_left(value, n, k);

                    break;
                }

                case 2: {
                    do {
                        std::cout << "Введите количество бит для сдвига k: (0.." << n << "): ";
                        std::cin >> k;

                    } while (k > n);

                    value = bit_utils::rotate_right(value, n, k);

                    break;
                }

                case 3: {
                    do {
                        std::cout << "Введите количество бит k в маске: (0.." << n << "): ";
                        std::cin >> k;

                    } while (k > n);

                    value = bit_utils::apply_mask(value, n, k);

                    break;
                }

                case 4: {
                    size_t i, j;
                    std::cout << "Введите i: ";
                    std::cin >> i;
                    std::cout << "Введите j: ";
                    std::cin >> j;

                    if (i > j || j >= n) {
                        std::cout << "Некорректные значения индексов.";
                        return;
                    }

                    std::cout << "Биты с " << i << " по " << j << ": ";
                    auto sub = bit_utils::get_bits(value, n, i, j);
                    bit_utils::print_bits(sub, n);
                    std::cout << std::endl;

                    break;
                }

                case 5: {
                    size_t i, j;
                    std::cout << "Введите i: ";
                    std::cin >> i;
                    std::cout << "Введите j: ";
                    std::cin >> j;

                    if (i >= n || j >= n) {
                        std::cout << "Некорректные значения индексов.";
                        return;
                    }

                    value = bit_utils::swap_bits(value, n, i, j);

                    break;
                }

                case 6: {
                    size_t i;
                    std::cout << "Введите i: ";
                    std::cin >> i;

                    int temp;
                    std::cout << "Введите новое значение i-го бита (0/1): ";
                    std::cin >> temp;

                    bool bit_value = temp == 1;
                    value = bit_utils::set_bit(value, n, i, bit_value);

                    break;
                }

                default:
                    break;
            }

        } while (choice != 0);
    }

    void bit_utils::print_bits(uint64_t const value, size_t n) {
        for (int i = static_cast<int>(n) - 1; i >= 0; --i) {
            std::cout << ((value >> i) & 1);
        }
    }

    uint64_t bit_utils::rotate_left(uint64_t value, size_t n, size_t k) {
        if (n == 0 || n > 64) throw std::invalid_argument("n должен принимать значения от 1 до 64");
        k %= n;

        uint64_t mask = n == 64 ? ~0ULL : (1ULL << n) - 1;
        value &= mask;

        return value << k | value >> (n - k) & mask;
    }

    uint64_t bit_utils::rotate_right(uint64_t value, size_t n, size_t k) {
        if (n == 0 || n > 64) throw std::invalid_argument("n должен принимать значения от 1 до 64");

        k %= n;
        uint64_t mask = n == 64 ? ~0ULL : (1ULL << n) - 1;
        value &= mask;
        return value >> k | value << (n - k) & mask;
    }

    uint64_t bit_utils::apply_mask(uint64_t value, size_t n, size_t k) {
        if (n == 0 || n > 64) throw std::invalid_argument("n должен принимать значения от 1 до 64");
        if (k > n) throw std::invalid_argument("k не может быть больше n");

        uint64_t nMask = n == 64 ? ~0ULL : (1ULL << n) - 1;
        value &= nMask;

        uint64_t kMask = k == 64 ? ~0ULL : (1ULL << k) - 1;
        return value & kMask;
    }

    uint64_t bit_utils::get_bits(uint64_t value, size_t n, size_t i, size_t j) {
        if (i > j || j >= n) throw std::invalid_argument("Некорректные начальный и последний индексы");
        size_t len = j - i + 1;
        uint64_t mask = (len == 64 ? ~0ULL : (1ULL << len) - 1);
        return value >> i & mask;
    }

    uint64_t bit_utils::swap_bits(uint64_t value, size_t n, size_t i, size_t j) {
        if (i >= n || j >= n) throw std::invalid_argument("Некорректные индексы заменяемых элементов");

        bool bit_i = value >> i & 1;
        bool bit_j = value >> j & 1;
        if (bit_i != bit_j) value ^= 1ULL << i | 1ULL << j;

        return value;
    }

    uint64_t bit_utils::set_bit(uint64_t value, size_t n, size_t i, bool bit_value) {
        if (i >= n) throw std::invalid_argument("Индекс заменяемого бита вне диапазона");

        if (bit_value) value |= (1ULL << i);
        else value &= ~(1ULL << i);

        return value;
    }
}
