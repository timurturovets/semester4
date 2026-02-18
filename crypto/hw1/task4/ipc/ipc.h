#pragma once

#include <windows.h>
#include <cstdint>
#include "../auxiliary/shared_memory.h"

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

constexpr size_t SHM_SIZE = sizeof(Request) + MAX_DATA_SIZE
                          + sizeof(Response) + MAX_DATA_SIZE;

constexpr wchar_t SHM_NAME[]      = L"RC4_SharedMemory";
constexpr wchar_t SLOT_SEM_NAME[] = L"RC4_SlotSemaphore";
constexpr wchar_t REQ_SEM_NAME[]  = L"RC4_RequestSemaphore";