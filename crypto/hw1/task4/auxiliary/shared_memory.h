#pragma once
#include <cstdint>

constexpr size_t MAX_SESSIONS = 100;
constexpr size_t MAX_KEY_SIZE = 256;
constexpr size_t MAX_DATA_SIZE = 5 * 1024 * 1024;

enum class Operation :uint32_t {
    Encrypt = 1,
    Decrypt = 2,
};

struct SessionSlot {
    uint32_t busy;
    uint32_t status;

    Operation op;

    uint32_t key_size;
    uint32_t data_size;

    uint8_t key[MAX_KEY_SIZE];
    uint8_t data[MAX_DATA_SIZE];
};

struct SharedMemory {
    SessionSlot slots[MAX_SESSIONS];
};

struct WorkerContext {
    SharedMemory *shared;
    HANDLE req_sem;
};