#pragma once

namespace tasks {
    class task1_aux {
    public:
        static int read_int(const std::string &prompt, int min_value, int max_value = std::numeric_limits<int>::max());
        static long long read_long_long(const std::string &prompt, long long min_value);
        static std::string read_alphabet(int r);
        static std::string read_pattern(int k, const std::string &alphabet);

        static std::string random_cluster(int n, const std::string &alphabet, std::mt19937 &rng);

        static bool is_connectable(const std::string &left, const std::string &right, const std::string &pattern);
    };
}