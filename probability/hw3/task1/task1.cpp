#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <limits>
#include <unordered_set>
#include <iomanip>

#include "task1.h"

#include "run.h"

namespace tasks {
    void task1::run(int argc, char **argv) {
        (void) argc;
        (void) argv;

        std::cout << "Эмпирическая оценка вероятностей для серии кластеров.\n";

        int m = task1_aux::read_int("Введите M (количество кластеров, M >= 2): ", 2);
        int n = task1_aux::read_int("Введите n (длина кластера, n >= 1): ", 1);
        int r = task1_aux::read_int("Введите r (мощность алфавита, r >= 1): ", 1);

        std::string alphabet = task1_aux::read_alphabet(r);

        int k = task1_aux::read_int("Введите k (длина паттерна, k >= 2): ", 2);


        std::string pattern = task1_aux::read_pattern(k, alphabet);
        int max_pairs = m - 1;
        int d = task1_aux::read_int("Введите D (число соединяемых пар соседних кластеров, 0..M-1): ", 0, max_pairs);
        long long trials = task1_aux::read_long_long("Введите число испытаний (>= 1): ", 1);

        if (k > 2 * n)  std::cout << "k > 2n, соединяемых пар быть не может.\n";

        std::random_device rd;
        std::mt19937 rng(rd());

        long long count_all = 0;
        long long count_d = 0;
        long long count_none = 0;

        std::vector<std::string> clusters;
        clusters.reserve(m);

        for (long long t = 0; t < trials; ++t) {
            clusters.clear();
            for (int i = 0; i < m; ++i) {
                clusters.push_back(task1_aux::random_cluster(n, alphabet, rng));
            }

            int connectable_pairs = 0;
            for (int i = 0; i < m - 1; ++i) {
                if (task1_aux::is_connectable(clusters[i], clusters[i + 1], pattern)) {
                    ++connectable_pairs;
                }
            }

            if (connectable_pairs == max_pairs) count_all++;

            if (connectable_pairs == d) count_d++;

            if (connectable_pairs == 0) count_none++;

        }

        auto probability = [trials](long long count) -> double {
            return static_cast<double>(count) / static_cast<double>(trials);
        };

        std::cout << std::fixed << std::setprecision(6);
        std::cout << "\nРезультаты (" << trials << " испытаний):\n";
        std::cout << "a) P(все соседние пары соединяемы) = " << probability(count_all) << "\n";
        std::cout << "b) P(ровно D соединяемых пар) = " << probability(count_d) << "\n";
        std::cout << "c) P(нет соединяемых пар) = " << probability(count_none) << "\n";
    }

    int task1_aux::read_int(const std::string &prompt, int min_value, int max_value) {
        while (true) {
            std::cout << prompt;
            int value = 0;
            if (!(std::cin >> value)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Ошибка ввода. Повторите.\n";
                continue;
            }
            if (value < min_value || value > max_value) {
                std::cout << "Значение должно быть в диапазоне [" << min_value << "; " << max_value << "].\n";
                continue;
            }
            return value;
        }
    }

    long long task1_aux::read_long_long(const std::string &prompt, long long min_value) {
        while (true) {
            std::cout << prompt;
            long long value = 0;
            if (!(std::cin >> value)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Ошибка ввода. Повторите.\n";
                continue;
            }
            if (value < min_value) {
                std::cout << "Значение должно быть не меньше " << min_value << ".\n";
                continue;
            }
            return value;
        }
    }

    std::string task1_aux::read_alphabet(int r) {
        while (true) {
            std::cout << "Введите алфавит (строка из " << r << " различных символов без пробелов): ";
            std::string alphabet;
            std::cin >> alphabet;
            if (alphabet.size() != r) {
                std::cout << "Длина строки должна быть " << r << ".\n";
                continue;
            }
            std::unordered_set<char> seen;
            bool unique = true;
            for (char c : alphabet) {
                if (!seen.insert(c).second) {
                    unique = false;
                    break;
                }
            }
            if (!unique) {
                std::cout << "Символы должны быть различными.\n";
                continue;
            }
            return alphabet;
        }
    }

    std::string task1_aux::read_pattern(int k, const std::string &alphabet) {
        std::unordered_set<char> allowed(alphabet.begin(), alphabet.end());
        while (true) {
            std::cout << "Введите k-паттерн (строка длины " << k << "): ";
            std::string pattern;
            std::cin >> pattern;
            if (static_cast<int>(pattern.size()) != k) {
                std::cout << "Длина паттерна должна быть " << k << ".\n";
                continue;
            }
            bool ok = true;
            for (char c : pattern) {
                if (allowed.find(c) == allowed.end()) {
                    ok = false;
                    break;
                }
            }
            if (!ok) {
                std::cout << "Паттерн должен состоять только из символов алфавита.\n";
                continue;
            }
            return pattern;
        }
    }

    std::string task1_aux::random_cluster(int n, const std::string &alphabet, std::mt19937 &rng) {
        std::uniform_int_distribution<int> dist(0, static_cast<int>(alphabet.size()) - 1);
        std::string cluster;
        cluster.reserve(n);
        for (int i = 0; i < n; ++i) {
            cluster.push_back(alphabet[dist(rng)]);
        }
        return cluster;
    }

    bool task1_aux::is_connectable(const std::string &left, const std::string &right, const std::string &pattern) {
        int k = static_cast<int>(pattern.size());
        int n_left = static_cast<int>(left.size());
        int n_right = static_cast<int>(right.size());

        for (int i = 1; i < k; ++i) {
            if (i > n_left || (k - i) > n_right) {
                continue;
            }

            bool ok = true;
            int left_start = n_left - i;
            for (int j = 0; j < i; ++j) {
                if (left[left_start + j] != pattern[j]) {
                    ok = false;
                    break;
                }
            }

            if (!ok) continue;

            for (int j = 0; j < k - i; ++j) {
                if (right[j] != pattern[i + j]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                return true;
            }
        }

        return false;
    }
}