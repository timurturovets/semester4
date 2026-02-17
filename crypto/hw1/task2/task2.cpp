#include <iostream>

#include "task2.h"

#include <bitset>

#include "run.h"

// ReSharper disable file CppLocalVariableMayBeConst
// ReSharper disable file CppTooWideScope

namespace tasks {
    void task2::run(int argc, char **argv) {
        size_t byte_count;
        std::cout << "Введите количество байт: ";
        std::cin >> byte_count;

        std::vector<uint8_t> data(byte_count);

        std::cout << "Введите " << byte_count << "байт (0-255) по одному в строке:" << std::endl;

        for (size_t i = 0; i < byte_count; ++i) {
            int temp;
            std::cin >> temp;

            if (temp < 0 || temp > 255) {
                std::cout << "Некорректное значение байта. Перезапустите программу.";
                return;
            }

            data[i] = static_cast<uint8_t>(temp);
        }

        size_t block_size;
        std::cout << "Введите размер S-блока (в битах): ";
        std:: cin >> block_size;

        auto sBlock = bit_substitutor::read_sBlock(block_size);

        auto result1 = bit_substitutor::substitute(data, block_size, sBlock);

        auto result2 = bit_substitutor::substitute(
            data,
            block_size,
            [block_size](size_t v) {
                return ((v << 1) | (v >> (block_size - 1))) & ((1 << block_size) - 1);
            });

        std::cout << "Результат через S-блок:" << std::endl;
        for (auto b : result1) {
            std::cout << std::bitset<8>(b) << " ";
        }

        std::cout << std::endl;

        std::cout << "Результат через функцию циклического сдвига:" << std::endl;
        for (auto b : result2) {
            std::cout << std::bitset<8>(b) << " ";
        }

        std::cout << std::endl;
    }

    std::vector<uint8_t> bit_substitutor::substitute(
        std::vector<uint8_t> const &data,
        size_t const block_size,
        std::unordered_map<size_t, size_t> const &sBlock
    ) {
        return substitute_inner(data, block_size,
            [&](size_t const value) {
                if (!sBlock.contains(value)) throw std::invalid_argument("Нет значения в S-блоке");
                return sBlock.at(value);
            });
    }

    std::vector<uint8_t> bit_substitutor::substitute(
        std::vector<uint8_t> const &data,
        size_t const block_size,
        std::function<size_t(size_t)> const &func
    ) {
        return substitute_inner(data, block_size, func);
    }

    std::vector<uint8_t> bit_substitutor::substitute_inner(
        std::vector<uint8_t> const &data,
        size_t block_size,
        std::function<size_t(size_t)> const &func
    ) {
        size_t total_bits = data.size() * 8;

        if (block_size == 0 || total_bits % block_size != 0) throw std::invalid_argument("Некорректный размер блока");

        size_t block_count = total_bits / block_size;

        std::vector<uint8_t> result(data.size(), 0);

        for (size_t b = 0; b < block_count; ++b) {
            size_t value = 0;
            for (size_t i = 0; i < block_size; ++i) {
                size_t bit_index = b * block_size + i;
                size_t byte_index = bit_index / 8;
                size_t bit_pos = bit_index % 8;

                size_t bit = (data[byte_index] >> bit_pos) & 1;
                value |= (bit << i);
            }

            size_t new_value = func(value);

            if (new_value >= (1ULL << block_size)) throw std::out_of_range("Значение вне диапазона блока");

            for (size_t i = 0; i < block_size; ++i) {
                size_t bit_index = b * block_size + i;
                size_t byte_index = bit_index / 8;
                size_t bit_pos = bit_index % 8;

                size_t bit = (new_value >> i) & 1;

                if (bit) result[byte_index] |= (1 << bit_pos);
                else result[byte_index] &= ~(1 << bit_pos);
            }
        }

        return result;
    }

    std::unordered_map<size_t, size_t> bit_substitutor::read_sBlock(size_t const block_size)
    {
        size_t n = 1ULL << block_size;
        std::unordered_map<size_t, size_t> sBlock;

        std::cout << "Введите " << n << " значений S-блока в формате вход выход" << std::endl;
        for (size_t i = 0; i < n; ++i) {
            size_t key, value;
            std::cin >> key >> value;

            if (key >= n || value >= n) {
                std::cerr << "Вход и выход должны быть меньше " << n << std::endl;
                --i;
                continue;
            }

            sBlock[key] = value;
        }

        return sBlock;
    }
}
