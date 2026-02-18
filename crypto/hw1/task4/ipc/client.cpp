#include <fstream>
#include <iostream>
#include <vector>
#include <numeric>
#include "ipc.h"
#include "../auxiliary/shared_memory.h"

namespace tasks {
    class client {
    public:
        static void run() {
            HANDLE slot_sem = OpenSemaphoreW(
                SYNCHRONIZE | SEMAPHORE_MODIFY_STATE,
                FALSE,
                SLOT_SEM_NAME
            );

            HANDLE req_sem = OpenSemaphoreW(
            SYNCHRONIZE | SEMAPHORE_MODIFY_STATE,
            FALSE,
                REQ_SEM_NAME
            );

            if (!slot_sem || !req_sem) {
                std::cerr << "Не удало  сь открыть семафоры";
                return;
            }

            HANDLE hMap = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, SHM_NAME);
            if (!hMap) {
                std::cerr << "Не удаось открыть file mapping";
                return;
            }

            void *view = MapViewOfFile(
                hMap,
                FILE_MAP_ALL_ACCESS,
                0,
                0,
                SHM_SIZE
            );

            if (!view) {
                std::cerr << "Не удалось выполнить MapViewOfFile";
                return;
            }

            auto *shared = reinterpret_cast<uint8_t *>(view);

            std::string file_path;
            std::cout << "Введите путь к шифруемому файлу:";
            std::getline(std::cin, file_path);

            std::ifstream fin(file_path, std::ios::binary);
            if (!fin) {
                std::cerr << "Не удалось открыть файл";
                close_ipc(view, hMap, slot_sem, req_sem);
                return;
            }

            int op_choice;
            std::cout << "1 - зашифровать файл, 2 - расшифровать файл: ";
            std::cin >> op_choice;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            std::vector<uint8_t> file_data((std::istreambuf_iterator(fin)), {});
            fin.close();

            if (file_data.size() > MAX_DATA_SIZE) {
                std::cerr << "Допустимый размер файла - до 10 МБ. Файл слишком большой";
                close_ipc(view, hMap, slot_sem, req_sem);
                return;
            }

            std::string key;
            std::cout << "Введите ключ для RC4:";
            std::getline(std::cin, key);
            std::vector<uint8_t> key_data(key.begin(), key.end());

            if (key_data.size() > MAX_KEY_SIZE) {
                std::cerr << "Слишком большой ключ.";
                close_ipc(view, hMap, slot_sem, req_sem);
                return;
            }

            WaitForSingleObject(slot_sem, INFINITE);

            size_t slot_idx = ULLONG_MAX;
            auto *shared_memory = reinterpret_cast<SharedMemory *>(shared);
            auto *slots = shared_memory->slots;
            for (size_t i = 0; i < MAX_SESSIONS; i++) {
                if (slots[i].busy == 0) {
                    slot_idx = i;
                    break;
                }
            }

            if (slot_idx == ULLONG_MAX) {
                std::cerr << "Свободного слота нет";
                close_ipc(view, hMap, slot_sem, req_sem);
                return;
            }

            auto *slot = &slots[slot_idx];
            slot->busy = 1;;
            slot->status = 1;
            slot->key_size = static_cast<uint32_t>(key_data.size());
            slot->data_size = static_cast<uint32_t>(file_data.size());
            slot->op = op_choice == 2 ? Operation::Decrypt : Operation::Encrypt;

            memcpy(slot->key, key_data.data(), key_data.size());
            memcpy(slot->data, file_data.data(), file_data.size());

            ReleaseSemaphore(req_sem, 1, nullptr);

            while (slot->status != 2) {
                Sleep(10);
            }

            std::cout << "Шифрование завершено. Размер данных: " << slot->data_size << " байт" << std::endl;

            std::string out_path = file_path + (slot->op == Operation::Decrypt ? ".dec" : ".enc");

            std::ofstream fout(out_path, std::ios::binary);
            fout.write(reinterpret_cast<char *>(slot->data), slot->data_size);
            fout.close();

            std::cout << "Результат записан в " << file_path << ".enc" << std::endl;

            slot->busy = 0;
            slot->status = 0;

            ReleaseSemaphore(slot_sem, 1, nullptr);

            close_ipc(view, hMap, slot_sem, req_sem);
        }

        static void close_ipc(void *view, HANDLE hMap, HANDLE slot_sem, HANDLE req_sem) {
            UnmapViewOfFile(view);
            CloseHandle(hMap);
            CloseHandle(slot_sem);
            CloseHandle(req_sem);
        }

        static bool run_file(std::string const &file_path, Operation op, std::string const &key) {
            HANDLE slot_sem = OpenSemaphoreW(
                SYNCHRONIZE | SEMAPHORE_MODIFY_STATE,
                FALSE,
                SLOT_SEM_NAME
            );

            HANDLE req_sem = OpenSemaphoreW(
                SYNCHRONIZE | SEMAPHORE_MODIFY_STATE,
                FALSE,
                REQ_SEM_NAME
            );

            if (!slot_sem || !req_sem) {
                std::cerr << "Не удалось открыть семафоры." << std::endl;
                return false;

            }

            HANDLE hMap = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, SHM_NAME);

            if (!hMap) {
                std::cout << "Не удалось создать FileMapping." << std::endl;
                return false;
            }

            void *view = MapViewOfFile(
                hMap,
                FILE_MAP_ALL_ACCESS,
                0,
                0,
                SHM_SIZE
            );

            if (!view) {
                std::cout << "Не удалось создать MapViewOfFile." << std::endl;
                return false;
            }

            auto *shared = reinterpret_cast<SharedMemory *>(view);
            auto *slots = shared->slots;

            std::ifstream fin(file_path, std::ios::binary);
            if (!fin) {
                close_ipc(view, hMap, slot_sem, req_sem);
                std::cerr << "Не удалось открыть файл." << std::endl;
                return false;
            }

            std::vector<uint8_t> file_data((
                std::istreambuf_iterator(fin)),
                std::istreambuf_iterator<char>()
            );

            fin.close();

            if (file_data.size() > MAX_DATA_SIZE) {
                close_ipc(view, hMap, slot_sem, req_sem);
                return false;
            }

            std::vector<uint8_t> key_data(key.begin(), key.end());

            WaitForSingleObject(slot_sem, INFINITE);

            size_t slot_idx = ULLONG_MAX;
            for (size_t i = 0; i < MAX_SESSIONS; i++) {
                if (slots[i].busy == 0) {
                    slot_idx = i;
                    break;
                }
            }

            if (slot_idx == ULLONG_MAX) {
                close_ipc(view, hMap, slot_sem, req_sem);
                return false;
            }

            SessionSlot *slot = &slots[slot_idx];

            slot->busy = 1;
            slot->status = 1;
            slot->key_size = static_cast<uint32_t>(key_data.size());
            slot->data_size = static_cast<uint32_t>(file_data.size());
            slot->op = op;

            memcpy(slot->key, key_data.data(), key_data.size());
            memcpy(slot->data, file_data.data(), file_data.size());

            ReleaseSemaphore(req_sem, 1, nullptr);

            while (slot->status != 2) {
                Sleep(1);
            }

            std::string out_path = file_path + (op == Operation::Encrypt ? ".enc" : ".dec");

            std::ofstream fout(out_path, std::ios::binary);
            fout.write(reinterpret_cast<char *>(slot->data), slot->data_size);
            fout.close();

            slot->busy = 0;
            slot->status = 0;

            ReleaseSemaphore(slot_sem, 1, nullptr);

            close_ipc(view, hMap, slot_sem, req_sem);

            return true;
        }
    };
}