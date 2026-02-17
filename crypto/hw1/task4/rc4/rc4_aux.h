#pragma once

#include <cstdint>

constexpr size_t MAX_DATA_SIZE = 10 * 1024 * 1024;

enum class Operation {
    Encrypt = 1,
    Decrypt = 2
};

struct Request {
    Operation op;
    uint32_t key_size;
    uint32_t data_size;
    uint8_t payload[];
};

struct Response {
    uint32_t data_size;
    uint8_t data[];
};
