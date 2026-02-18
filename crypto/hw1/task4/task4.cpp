#include <windows.h>
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
#include <thread>

#include "run.h"
#include "ipc/client.cpp"
#include "ipc/server.cpp"

namespace tasks {
    static volatile LONG rc4_application_running = 1;
    static bool compare_files(std::string const &f1 , std::string const &f2);

    void task4::run(int argc, char **argv) {
        std::thread server_thread([]() {
            server::run();
        });

        bool server_ready = false;
        for (int i = 0; i < 30; i++) {
            HANDLE hMap = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, SHM_NAME);
            if (hMap) {
                CloseHandle(hMap);
                server_ready = true;
                break;
            }
            Sleep(100);
        }
        if (!server_ready) {
            std::cerr << "Сервер не запустился.\n";
            server_thread.detach();
            return;
        }

        int choice = -1;
        while (choice != 0) {
            //system("cls");
            std::cout << "1. Проверить функцию шифрования на одном файле\n";
            std::cout << "2. Моделирование 100 одновременных сессий шифрования\n";
            std::cout << "0. Выход\n";
            std::cout << "Выберите задачу: ";
            std::cin >> choice;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            switch (choice) {
                case 1:
                    run_example();
                    break;
                case 2:
                    run_modeling();
                    break;
                case 0:
                    break;
                default:
                    std::cout << "Неверный выбор, попробуйте снова.\n";
                    Sleep(1000);
            }

            std::cout << "Успешно." << std::endl;
            std::cin.get();
        }

        rc4_application_running = 0;

        HANDLE req_sem = OpenSemaphoreW(
            SYNCHRONIZE | SEMAPHORE_MODIFY_STATE,
            FALSE,
            REQ_SEM_NAME
        );

        if (req_sem) {
            ReleaseSemaphore(req_sem, MAX_SESSIONS, nullptr);
            CloseHandle(req_sem);
        }

        server_thread.join();
    }


    void task4::run_example() {
        std::string file;
        std::cout << "Введите путь к файлу: ";
        std::cin >> file;

        std::string key;
        std::cout << "Введите ключ: ";
        std::cin >> key;

        if (!client::run_file(file, Operation::Encrypt, key)) {
            std::cout << "Ошибка при шифровании файла." << std::endl;
            return;
        }

        if (!client::run_file(file + ".enc", Operation::Decrypt, key)) {
            std::cout << "Ошибка при расшифровании файла." << std::endl;;
            return;
        }

        if (compare_files(file, file + ".enc.dec")) std::cout << "Файл успешно зашифрован и расшифрован." << std::endl;
        else std::cout << "Ошибка: исходный файл не совпадает с итоговым" << std::endl;
    }

    void task4::run_modeling() {
        constexpr size_t THREADS = MAX_SESSIONS;
        std::vector<std::thread> threads;

        for (size_t i = 0; i < THREADS; i++) {
            threads.emplace_back([i]() {
                std::string file = "model_" + std::to_string(i) + ".txt";

                std::string key = "key_" + std::to_string(i);

                std::ofstream(file, std::ios::binary) << "Thread data " << i;

                client::run_file(file, Operation::Encrypt, key);
                client::run_file(file + ".enc", Operation::Decrypt, key);

                if (!compare_files(file, file + ".enc.dec")) {
                    std::cerr << "В потоке " << i << " файлы не совпадают" << std::endl;
                }
            });
        }

        for (auto &t : threads) {
            t.join();
        }

        std::cout << "Моделирование 100 сессий прошло успешно.";
    }

    static bool compare_files(std::string const &f1 , std::string const &f2) {
        std::ifstream file1(f1, std::ios::binary | std::ios::ate);
        std::ifstream file2(f2, std::ios::binary | std::ios::ate);

        if (!file1 || !file2) return false;

        if (file1.tellg() != file2.tellg()) return false;

        file1.seekg(0);
        file2.seekg(0);

        constexpr size_t BUF_SIZ = 4096;
        char buf1[BUF_SIZ];
        char buf2[BUF_SIZ];

        while (file1 && file2) {
            file1.read(buf1, BUF_SIZ);
            file2.read(buf2, BUF_SIZ);

            if (memcmp(buf1, buf2, file1.gcount()) != 0) return false;
        }

        return true;
    }
}
