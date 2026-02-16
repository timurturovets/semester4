#pragma once
#include <iostream>
#include <vector>

#include "task1/run.h"


namespace tasks {
    class menu {
    public:
        static void start(int argc, char **argv) {
            std::vector const runners = {
                task1::run,
            };

            while (true) {
                std::system("cls");
                std::cout << "1. Перестановка битов в рамках переданного значения" << std::endl;

                std::cout << "0. Выход" << std::endl;

                int choice = -1;
                do {
                    std::cout << std::endl << "Выберите задачу: ";
                    std::cin >> choice;
                } while (choice < 0 || choice > runners.size());

                if (choice == 0) return;
                try {
                    runners[choice - 1](argc, argv);
                } catch (std::exception &e) {
                    std::cout << "Произошла ошибка: " << e.what() << std::endl;
                    std::cout << "Попробуйте перезапустить программу с другими входными значениями.";
                }

                std::cout << std::endl << "Введите любое значение, чтобы продолжить...";
                char stub;
                std::cin >> stub;
            }
        }
    };
}
