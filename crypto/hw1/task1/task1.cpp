#include <iostream>
#include <stdexcept>
#include <bitset>

#include "task1.h"

#include "run.h"

namespace tasks {
    void task1::run(int argc, char **argv) {
        size_t byte_count;
        do {
            std::cout << "Введите количество байт: ";
            std::cin >> byte_count;
        } while (byte_count == 0);

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

        size_t total_bits = byte_count * 8;

        std::vector<size_t> pBlock(total_bits);

        std::cout << "Введите P-блок (" << total_bits << " индексов):" << std::endl;

        for (size_t i = 0; i < total_bits; ++i) {
            std::cin >> pBlock[i];
        }

        int order_input;
        std::cout << "Введите порядок битов (0 - LSB_FIRST, 1 - MSB_FIRST): ";
        std::cin >> order_input;

        bit_order order = order_input == 1 ? bit_order::MSB_FIRST : bit_order::LSB_FIRST;

        int base_input;
        std::cout << "Введите базу индекса (0 - ZERO_BASED, 1 - ONE_BASED): ";
        std::cin >> base_input;

        index_base const base = base_input == 1 ? index_base::ONE_BASED: index_base::ZERO_BASED;

        auto const result = bit_permutator::permute(data, pBlock, order, base);

        std::cout << "Результат в битах: " << std::endl;

        for (auto const i : result) {
            std::cout << std::bitset<8>(i) << std::endl;
        }

        std::cout << std::endl << "Результат в десятичном виде: " << std::endl;
        for (auto const i : result) {
            std::cout << static_cast<int>(i) << " ";
        }

        std::cout << std::endl;
    }

    std::vector<uint8_t> bit_permutator::permute(
        std::vector<uint8_t> const &data,
        std::vector<size_t> const &pBlock,
        bit_order const order,
        index_base const base
    ) {
        size_t const total_bits = data.size() * 8;

        if (pBlock.size() != total_bits) {
            throw std::invalid_argument("Размер P-блока не совпадает с количеством бит");
        }

        std::vector<uint8_t> result(data.size(), 0);

        for (size_t i = 0; i < total_bits; ++i) {
            size_t src_index = pBlock[i];

            if (base == index_base::ONE_BASED) {
                if (src_index == 0) {
                    throw std::invalid_argument("Индекс не может быть 0 при индексации с единицы");
                }
                src_index -= 1;
            }

            if (src_index >= total_bits) throw std::out_of_range("Индекс P-блока вне диапазона");
            bool const bit = getBit(data, src_index, order);
            setBit(result, i, bit, order);
        }

        return result;
    }

    bool bit_permutator::getBit(
        std::vector<uint8_t> const &data,
        size_t const bit_index,
        bit_order const order
    ) {
        size_t const byte_index = bit_index / 8;
        size_t bit_pos = bit_index % 8;

        if (order == bit_order::MSB_FIRST) bit_pos = 7 - bit_pos;

        return data[byte_index] >> bit_pos & 1;
    }

    void bit_permutator::setBit(
        std::vector<uint8_t> &data,
        size_t const bit_index,
        bool const value,
        bit_order const order
    ) {
        size_t const byte_index = bit_index / 8;
        size_t bit_pos = bit_index % 8;

        if (order == bit_order::MSB_FIRST) bit_pos = 7 - bit_pos;

        if (value) data[byte_index] |= 1 << bit_pos;
        else data[byte_index] &= ~(1 << bit_pos);
    }
}
