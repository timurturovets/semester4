#include "task1.h"
#include "run.h"

#include <iostream>
#include <stdexcept>

namespace tasks {
    int read_choice() {
        while (true) {
            std::cout << std::endl << "Выберите операцию: ";
            int choice = -1;

            if (std::cin >> choice) return choice;

            std::cout << "Некорректный ввод. Введите заново:" << std::endl;
            std::cin.clear();
            std::cin.ignore(10000, '\n');
        }
    }

    std::int64_t read_int64(const char *prompt) {
        while (true) {
            std::cout << prompt;
            std::int64_t value = 0;

            if (std::cin >> value) return value;

            std::cout << "Некорректный ввод. Введите заново:" << std::endl;
            std::cin.clear();
            std::cin.ignore(10000, '\n');
        }
    }

    void task1::run(int argc, char **argv) {
        while (true) {
            std::cout << "1. Вычислить символ Лежандра" << std::endl;
            std::cout << "2. Вычислить символ Якоби" << std::endl;
            std::cout << "3. Вычислить НОД (алгоритм Евклида)" << std::endl;
            std::cout << "4. Вычислить НОД и найти коэффициенты Безу (расширенный алгоритм Евклида)" << std::endl;
            std::cout << "5. Возвести в степень по модулю" << std::endl;
            std::cout << "6. Найти мультипликативный обратный элемент в Z_n" << std::endl;
            std::cout << "7. Вычислить функцию Эйлера φ(n)" << std::endl;
            std::cout << "0. В меню" << std::endl;

            const int choice = read_choice();

            if (choice == 0) return;

            try {
                if (choice == 1) {
                    const std::int64_t a = read_int64("Введите a: ");
                    const std::int64_t p = read_int64("Введите p (нечетное простое): ");

                    const int value = task1_service::legendre_symbol(a, p);

                    std::cout << "Символ Лежандра (" << a << "/" << p << ") = " << value << std::endl;

                    continue;
                }

                if (choice == 2) {
                    const std::int64_t a = read_int64("Введите a: ");
                    const std::int64_t n = read_int64("Введите n (нечетное > 1): ");

                    const int value = task1_service::jacobi_symbol(a, n);

                    std::cout << "Символ Якоби (" << a << "/" << n << ") = " << value << std::endl;

                    continue;
                }

                if (choice == 3) {
                    const std::int64_t a = read_int64("Введите первое целое число: ");
                    const std::int64_t b = read_int64("Введите второе целое число: ");

                    const std::int64_t value = task1_service::gcd_euclid(a, b);

                    std::cout << "НОД(" << a << ", " << b << ") = " << value << std::endl;

                    continue;
                }

                if (choice == 4) {
                    const std::int64_t a = read_int64("Введите первое целое число: ");
                    const std::int64_t b = read_int64("Введите второе целое число: ");

                    const auto result = task1_service::gcd_euclid_extended(a, b);

                    std::cout << "НОД(" << a << ", " << b << ") = " << result.gcd << std::endl;
                    std::cout << "Коэффициенты Безу: x = " << result.x << ", y = " << result.y << std::endl;
                    std::cout << "Соотношение Безу: " << a << " * " << result.x << " + " << b << " * " << result.y << " = " << result.gcd << std::endl;

                    continue;
                }

                if (choice == 5) {
                    const std::int64_t base = read_int64("Введите основание: ");
                    const std::int64_t exponent = read_int64("Введите показатель степени (>= 0): ");
                    const std::int64_t mod = read_int64("Введите модуль (> 0): ");

                    const std::int64_t value = task1_service::pow_mod(base, exponent, mod);

                    std::cout << "(" << base << "^" << exponent << ") mod " << mod << " = " << value << std::endl;

                    continue;
                }

                if (choice == 6) {
                    const std::int64_t value = read_int64("Введите значение из Z_n: ");
                    const std::int64_t n = read_int64("Введите n (> 1): ");

                    const std::int64_t inverse = task1_service::mod_inverse(value, n);

                    std::cout << "Обратный элемент: " << inverse << std::endl;
                    std::cout << "Проверка: (" << value << " * " << inverse << ") mod " << n << " = 1" << std::endl;

                    continue;
                }

                if (choice == 7) {
                    const std::int64_t n = read_int64("Введите n (> 0): ");

                    std::cout << "1. По определению" << std::endl;
                    std::cout << "2. По факторизации (основная теорема арифметики)" << std::endl;
                    std::cout << "3. Через сумму с cos (дискретное преобразование Фурье)" << std::endl;

                    const int method = read_choice();

                    if (method == 1) {
                        const std::int64_t value = task1_service::euler_phi_definition(n);
                        std::cout << "φ(" << n << ") = " << value << std::endl;
                        continue;
                    }

                    if (method == 2) {
                        const std::int64_t value = task1_service::euler_phi_factorization(n);
                        std::cout << "φ(" << n << ") = " << value << std::endl;
                        continue;
                    }

                    if (method == 3) {
                        const std::int64_t value = task1_service::euler_phi_fourier(n);
                        std::cout << "φ(" << n << ") = " << value << std::endl;
                        continue;
                    }

                    std::cout << "Неизвестный метод." << std::endl;
                    continue;
                }

                std::cout << "Неизвестный пункт меню." << std::endl;
            } catch (const std::exception &e) {
                std::cout << "Ошибка: " << e.what() << std::endl;
            }
        }
    }
}
