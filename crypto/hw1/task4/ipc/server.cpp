#include <windows.h>
#include <iostream>
#include <thread>
#include <vector>

#include "../auxiliary/shared_memory.h"
#include "../rc4/rc4.h"

#include "ipc.h"

namespace tasks {

    class server {
    public:
        static void run() {
            size_t shmSize = sizeof(SharedMemory);

            HANDLE hMap = CreateFileMappingW(
                INVALID_HANDLE_VALUE,
                nullptr,
                PAGE_READWRITE,
                0,
                (DWORD)shmSize,
                SHM_NAME
            );

            DWORD err = GetLastError();
            bool first_run = false;

            if (!hMap) {
                std::cerr << "CreateFileMappingW failed: " << err << std::endl;
                return;
            }

            if (err == ERROR_ALREADY_EXISTS) {
                std::cout << "Файл маппинга уже существует." << std::endl;
            } else {
                first_run = true;
            }

            void* view = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, shmSize);
            if (!view) {
                std::cerr << "MapViewOfFile failed: " << GetLastError() << std::endl;
                CloseHandle(hMap);
                return;
            }

            auto* shared = reinterpret_cast<SharedMemory*>(view);
            if (first_run) ZeroMemory(shared, shmSize);


            HANDLE slot_sem = CreateSemaphoreW(nullptr, MAX_SESSIONS, MAX_SESSIONS, SLOT_SEM_NAME);
            if (!slot_sem) {
                std::cerr << "CreateSemaphoreW slot failed: " << GetLastError() << std::endl;
                UnmapViewOfFile(view);
                CloseHandle(hMap);
                return;
            }

            HANDLE req_sem = CreateSemaphoreW(nullptr, 0, MAX_SESSIONS, REQ_SEM_NAME);
            if (!req_sem) {
                std::cerr << "CreateSemaphoreW req failed: " << GetLastError() << std::endl;
                CloseHandle(slot_sem);
                UnmapViewOfFile(view);
                CloseHandle(hMap);
                return;
            }

            std::cout << "Сервер запущен." << std::endl;

            constexpr size_t THREAD_COUNT = MAX_SESSIONS;

            WorkerContext ctx{};
            ctx.shared = shared;
            ctx.req_sem = req_sem;

            std::vector<HANDLE> workers;

            for (size_t i = 0; i < THREAD_COUNT; i++) {
                HANDLE hThread = CreateThread(
                  nullptr,
                  0,
                  worker_proc,
                  &ctx,
                  0,
                  nullptr
                );

                if (hThread) workers.push_back(hThread);
            }

            std::cout << "Server listening" << std::endl;
            WaitForMultipleObjects(
                workers.size(),
                workers.data(),
                TRUE,
                INFINITE
            );

            UnmapViewOfFile(view);
            CloseHandle(hMap);
            CloseHandle(slot_sem);
            CloseHandle(req_sem);
        }

        static DWORD WINAPI worker_proc(LPVOID param) {
            auto *ctx = reinterpret_cast<WorkerContext *>(param);
            auto *shared = ctx->shared;
            HANDLE req_sem = ctx->req_sem;

            while (true) {
                WaitForSingleObject(req_sem, INFINITE);

                for (size_t i = 0; i < MAX_SESSIONS; i++) {
                    SessionSlot &slot =  shared->slots[i];

                    if (slot.busy == 1 && slot.status == 1) {
                        if (InterlockedCompareExchange(
                            reinterpret_cast<LONG *>(&slot.status),
                            3,
                            1
                        ) != 1) continue;

                        std::vector<uint8_t> key(slot.key, slot.key + slot.key_size);
                        std::vector <uint8_t> data(slot.data, slot.data + slot.data_size);

                        try {
                            RC4 rc4(key);
                            rc4.process(data);

                            memcpy(slot.data, data.data(), slot.data_size);
                            slot.status = 2;
                        } catch (...) {
                            slot.status = 0;
                            slot.busy = 0;
                        }

                        break;
                    }
                }
            }
        }
    };
}