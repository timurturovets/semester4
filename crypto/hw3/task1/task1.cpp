#include "task1.h"
#include "run.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>

#include <sstream>
#include <stdexcept>
#include <thread>

namespace tasks {
    std::size_t normalize_threads_count(std::size_t threads_count) {
        return std::max<std::size_t>(1, threads_count);
    }

    std::vector<std::uint8_t> normalize_iv(const std::vector<std::uint8_t> &iv,
                                           std::size_t block_size) {
        if (block_size == 0) throw std::runtime_error("Размер блока должен быть больше нуля.");

        if (iv.empty()) return std::vector<std::uint8_t>(block_size, 0);

        if (iv.size() != block_size) throw std::runtime_error("Размер IV должен совпадать с размером блока.");

        return iv;
    }

    class zeros_padding final : public i_padding_mode {
    public:
        [[nodiscard]] std::vector<std::uint8_t> pad(const std::vector<std::uint8_t> &input,
                                      std::size_t block_size) const override {
            if (block_size == 0) throw std::runtime_error("Размер блока должен быть больше нуля.");

            std::vector<std::uint8_t> output = input;
            const std::size_t remainder = input.size() % block_size;

            if (remainder == 0) return output;

            output.resize(input.size() + (block_size - remainder), 0);

            return output;
        }

        void unpad(std::vector<std::uint8_t> &input, std::size_t block_size) const override {
            if (block_size == 0) throw std::runtime_error("Размер блока должен быть больше нуля.");

            while (!input.empty() && input.back() == 0) {
                input.pop_back();
            }
        }
    };

    class ansi_x923_padding final : public i_padding_mode {
    public:
        [[nodiscard]] std::vector<std::uint8_t> pad(const std::vector<std::uint8_t> &input,
                                      std::size_t block_size) const override {
            if (block_size == 0 || block_size > 255) throw std::runtime_error("ANSI X.923 требует размер блока от 1 до 255 байт.");

            const std::size_t remainder = input.size() % block_size;
            std::size_t pad_size = block_size - remainder;

            if (pad_size == 0) pad_size = block_size;

            std::vector<std::uint8_t> output = input;

            output.resize(input.size() + pad_size, 0);
            output.back() = static_cast<std::uint8_t>(pad_size);

            return output;
        }

        void unpad(std::vector<std::uint8_t> &input, std::size_t block_size) const override {
            if (input.empty()) throw std::runtime_error("Пустые данные не могут быть распакованы ANSI X.923.");

            if (block_size == 0 || block_size > 255) throw std::runtime_error("ANSI X.923 требует размер блока от 1 до 255 байт.");

            if (input.size() % block_size != 0) throw std::runtime_error("Размер данных должен быть кратен размеру блока.");

            const std::size_t pad_size = input.back();

            if (pad_size == 0
                || pad_size > block_size
                || pad_size > input.size()
            ) throw std::runtime_error("Некорректные данные набивки ANSI X.923.");

            for (std::size_t i = input.size() - pad_size; i + 1 < input.size(); ++i) {
                if (input[i] != 0) throw std::runtime_error("Некорректные данные набивки ANSI X.923.");
            }

            input.resize(input.size() - pad_size);
        }
    };

    class ecb_mode final : public i_cipher_mode {
    public:
        void encrypt(const i_symmetric_block_cipher &algorithm,
                     const std::vector<std::uint8_t> &input,
                     std::vector<std::uint8_t> &output,
                     const std::vector<std::uint8_t> &iv,
                     std::size_t threads_count,
                     const std::vector<std::size_t> &mode_params) const override {
            (void) iv;
            (void) mode_params;

            const std::size_t block_size = algorithm.block_size();
            if (block_size == 0 || input.size() % block_size != 0) {
                throw std::runtime_error("Размер входных данных должен быть кратен размеру блока.");
            }

            output.resize(input.size());
            const std::size_t blocks_count = input.size() / block_size;
            task1_aux::parallel_for_blocks(blocks_count, threads_count,
               [&](std::size_t from, std::size_t to) {
                   for (std::size_t block = from; block < to; ++block) {
                       const std::size_t offset = block * block_size;

                       algorithm.encrypt_block(input.data() + offset, output.data() + offset);
                   }
               });
        }

        void decrypt(const i_symmetric_block_cipher &algorithm,
                     const std::vector<std::uint8_t> &input,
                     std::vector<std::uint8_t> &output,
                     const std::vector<std::uint8_t> &iv,
                     std::size_t threads_count,
                     const std::vector<std::size_t> &mode_params) const override {
            (void) iv;
            (void) mode_params;

            const std::size_t block_size = algorithm.block_size();
            if (block_size == 0
                || input.size() % block_size != 0
                ) throw std::runtime_error("Размер входных данных должен быть кратен размеру блока.");

            output.resize(input.size());

            const std::size_t blocks_count = input.size() / block_size;
            task1_aux::parallel_for_blocks(blocks_count, threads_count,
               [&](std::size_t from, std::size_t to) {
                   for (std::size_t block = from; block < to; ++block) {
                       const std::size_t offset = block * block_size;

                       algorithm.decrypt_block(input.data() + offset, output.data() + offset);
                   }
               });
        }
    };

    class cbc_mode final : public i_cipher_mode {
    public:
        void encrypt(const i_symmetric_block_cipher &algorithm,
                     const std::vector<std::uint8_t> &input,
                     std::vector<std::uint8_t> &output,
                     const std::vector<std::uint8_t> &iv,
                     std::size_t threads_count,
                     const std::vector<std::size_t> &mode_params) const override {
            (void) threads_count;
            (void) mode_params;

            const std::size_t block_size = algorithm.block_size();
            if (block_size == 0
                || input.size() % block_size != 0
                ) throw std::runtime_error("Размер входных данных должен быть кратен размеру блока.");

            const std::vector<std::uint8_t> init_vector = normalize_iv(iv, block_size);

            output.resize(input.size());

            std::vector<std::uint8_t> feedback = init_vector;
            std::vector<std::uint8_t> mixed(block_size);

            const std::size_t blocks_count = input.size() / block_size;

            for (std::size_t block = 0; block < blocks_count; ++block) {
                const std::size_t offset = block * block_size;
                const auto *plain_block = input.data() + offset;
                auto *cipher_block = output.data() + offset;

                task1_aux::xor_blocks(plain_block, feedback.data(), mixed.data(), block_size);

                algorithm.encrypt_block(mixed.data(), cipher_block);

                std::copy(cipher_block, cipher_block + block_size, feedback.begin());
            }
        }

        void decrypt(const i_symmetric_block_cipher &algorithm,
                     const std::vector<std::uint8_t> &input,
                     std::vector<std::uint8_t> &output,
                     const std::vector<std::uint8_t> &iv,
                     std::size_t threads_count,
                     const std::vector<std::size_t> &mode_params) const override {
            (void) mode_params;

            const std::size_t block_size = algorithm.block_size();
            if (block_size == 0 || input.size() % block_size != 0) {
                throw std::runtime_error("Размер входных данных должен быть кратен размеру блока.");
            }

            const std::vector<std::uint8_t> init_vector = normalize_iv(iv, block_size);

            output.resize(input.size());

            const std::size_t blocks_count = input.size() / block_size;

            task1_aux::parallel_for_blocks(blocks_count, threads_count,
               [&](std::size_t from, std::size_t to) {
                   std::vector<std::uint8_t> decrypted(block_size);

                   for (std::size_t block = from; block < to; ++block) {
                       const std::size_t offset = block * block_size;
                       const auto *cipher_block = input.data() + offset;

                       auto *plain_block = output.data() + offset;

                       algorithm.decrypt_block(cipher_block, decrypted.data());

                       const auto *feedback =
                           block == 0
                              ? init_vector.data()
                              : input.data() + offset - block_size;

                       task1_aux::xor_blocks(decrypted.data(), feedback, plain_block, block_size);
                   }
               }
            );
        }
    };

    class pcbc_mode final : public i_cipher_mode {
    public:
        void encrypt(const i_symmetric_block_cipher &algorithm,
                     const std::vector<std::uint8_t> &input,
                     std::vector<std::uint8_t> &output,
                     const std::vector<std::uint8_t> &iv,
                     std::size_t threads_count,
                     const std::vector<std::size_t> &mode_params) const override {
            (void) threads_count;
            (void) mode_params;

            const std::size_t block_size = algorithm.block_size();

            if (block_size == 0
                || input.size() % block_size != 0
                ) throw std::runtime_error("Размер входных данных должен быть кратен размеру блока.");

            const std::vector<std::uint8_t> init_vector = normalize_iv(iv, block_size);
            output.resize(input.size());

            std::vector<std::uint8_t> feedback = init_vector;
            std::vector<std::uint8_t> mixed(block_size);
            std::vector<std::uint8_t> next_feedback(block_size);

            const std::size_t blocks_count = input.size() / block_size;

            for (std::size_t block = 0; block < blocks_count; ++block) {
                const std::size_t offset = block * block_size;
                const auto *plain_block = input.data() + offset;
                auto *cipher_block = output.data() + offset;

                task1_aux::xor_blocks(plain_block, feedback.data(), mixed.data(), block_size);
                algorithm.encrypt_block(mixed.data(), cipher_block);

                task1_aux::xor_blocks(plain_block, cipher_block, next_feedback.data(), block_size);
                feedback.swap(next_feedback);
            }
        }

        void decrypt(const i_symmetric_block_cipher &algorithm,
                     const std::vector<std::uint8_t> &input,
                     std::vector<std::uint8_t> &output,
                     const std::vector<std::uint8_t> &iv,
                     std::size_t threads_count,
                     const std::vector<std::size_t> &mode_params) const override {
            (void) threads_count;
            (void) mode_params;

            const std::size_t block_size = algorithm.block_size();

            if (block_size == 0
                || input.size() % block_size != 0
                ) throw std::runtime_error("Размер входных данных должен быть кратен размеру блока.");

            const std::vector<std::uint8_t> init_vector = normalize_iv(iv, block_size);
            output.resize(input.size());

            std::vector<std::uint8_t> feedback = init_vector;
            std::vector<std::uint8_t> decrypted(block_size);
            std::vector<std::uint8_t> next_feedback(block_size);

            const std::size_t blocks_count = input.size() / block_size;

            for (std::size_t block = 0; block < blocks_count; ++block) {
                const std::size_t offset = block * block_size;
                const auto *cipher_block = input.data() + offset;
                auto *plain_block = output.data() + offset;

                algorithm.decrypt_block(cipher_block, decrypted.data());
                task1_aux::xor_blocks(decrypted.data(), feedback.data(), plain_block, block_size);

                task1_aux::xor_blocks(plain_block, cipher_block, next_feedback.data(), block_size);
                feedback.swap(next_feedback);
            }
        }
    };

    class xor_block_cipher final : public i_symmetric_block_cipher {
    public:
        static constexpr std::size_t kBlockSize = 16;

        [[nodiscard]] std::size_t block_size() const override { return kBlockSize; }

        void set_key(const std::vector<std::uint8_t> &key) override {
            if (key.empty()) throw std::runtime_error("Ключ не должен быть пустым.");

            key_ = key;
        }

        void encrypt_block(const std::uint8_t *input, std::uint8_t *output) const override {
            if (key_.empty()) throw std::runtime_error("Ключ алгоритма не инициализирован.");

            for (std::size_t i = 0; i < kBlockSize; ++i) {
                output[i] = static_cast<std::uint8_t>(input[i] ^ key_[i % key_.size()]);
            }
        }

        void decrypt_block(const std::uint8_t *input, std::uint8_t *output) const override {
            encrypt_block(input, output);
        }

    private:
        std::vector<std::uint8_t> key_;
    };

    std::unique_ptr<i_cipher_mode> create_mode(cipher_mode mode) {
        switch (mode) {
            case cipher_mode::ECB:
                return std::make_unique<ecb_mode>();
            case cipher_mode::CBC:
                return std::make_unique<cbc_mode>();
            case cipher_mode::PCBC:
                return std::make_unique<pcbc_mode>();
            default:
                throw std::runtime_error("Неизвестный режим шифрования.");
        }
    }

    std::unique_ptr<i_padding_mode> create_padding(padding_mode padding) {
        switch (padding) {
            case padding_mode::Zeros:
                return std::make_unique<zeros_padding>();
            case padding_mode::ANSI_X923:
                return std::make_unique<ansi_x923_padding>();
            default:
                throw std::runtime_error("Неизвестный режим набивки.");
        }
    }

    int read_choice(const std::string &prompt, int from, int to) {
        int value = from;

        while (true) {
            std::cout << prompt;

            if (!(std::cin >> value)) {
                std::cout << "Ошибка ввода. Повторите попытку." << std::endl;
                continue;
            }

            if (value >= from && value <= to) return value;

            std::cout << "Значение вне диапазона. Повторите попытку." << std::endl;
        }
    }

    std::size_t read_threads_count() {
        std::size_t threads_count = 1;

        while (true) {
            std::cout << "Количество потоков (>=1): ";
            if (!(std::cin >> threads_count)) {
                std::cout << "Ошибка ввода. Повторите попытку." << std::endl;
                continue;
            }

            if (threads_count >= 1) return threads_count;

            std::cout << "Количество потоков должно быть не меньше 1." << std::endl;
        }
    }

    void task1_aux::parallel_for_blocks(
            const std::size_t blocks_count,
        std::size_t threads_count,
        const std::function<void(std::size_t, std::size_t)> &worker
    ) {
        if (blocks_count == 0) return;


        threads_count = normalize_threads_count(threads_count);
        threads_count = std::min<std::size_t>(threads_count, blocks_count);

        if (threads_count == 1) {
            worker(0, blocks_count);
            return;
        }

        const std::size_t chunk = blocks_count / threads_count;
        const std::size_t remainder = blocks_count % threads_count;

        std::vector<std::thread> threads;
        threads.reserve(threads_count);

        std::vector<std::exception_ptr> exceptions(threads_count);
        std::size_t begin = 0;

        for (std::size_t i = 0; i < threads_count; ++i) {
            const std::size_t length = chunk + (i < remainder ? 1 : 0);
            const std::size_t end = begin + length;

            threads.emplace_back([&, i, begin, end]() {
                try {
                    worker(begin, end);
                } catch (...) {
                    exceptions[i] = std::current_exception();
                }
            });

            begin = end;
        }

        for (auto &thread: threads) {
            thread.join();
        }

        for (const auto &error: exceptions) {
            if (error != nullptr) {
                std::rethrow_exception(error);
            }
        }
    }

    std::vector<std::uint8_t> task1_aux::read_file_bytes(const std::filesystem::path &path) {
        std::ifstream input(path, std::ios::binary);
        if (!input) throw std::runtime_error("Не удалось открыть входной файл: " + path.string());

        input.seekg(0, std::ios::end);
        const auto end_position = input.tellg();

        if (end_position < 0) {
            throw std::runtime_error("Не удалось определить размер файла: " + path.string());
        }

        const auto size = static_cast<std::size_t>(end_position);
        std::vector<std::uint8_t> data(size);
        input.seekg(0, std::ios::beg);

        if (size > 0) {
            input.read(reinterpret_cast<char *>(data.data()), static_cast<std::streamsize>(size));
            if (!input) throw std::runtime_error("Ошибка чтения файла: " + path.string());
        }

        return data;
    }

    void task1_aux::write_file_bytes(
        const std::filesystem::path &path,
        const std::vector<std::uint8_t> &bytes
    ) {
        std::ofstream output(path, std::ios::binary | std::ios::trunc);

        if (!output) throw std::runtime_error("Не удалось открыть выходной файл: " + path.string());

        if (!bytes.empty()) {
            output.write(
                reinterpret_cast<const char *>(bytes.data()),
                static_cast<std::streamsize>(bytes.size())
            );

            if (!output) throw std::runtime_error("Ошибка записи файла: " + path.string());
        }
    }

    void task1_aux::xor_blocks(
        const std::uint8_t *left,
       const std::uint8_t *right,
       std::uint8_t *result,
       const std::size_t block_size
    ) {
        for (std::size_t i = 0; i < block_size; ++i) {
            result[i] = static_cast<std::uint8_t>(left[i] ^ right[i]);
        }
    }

    std::vector<std::uint8_t> task1_aux::parse_hex_string(const std::string &hex_string) {
        std::string compact_hex;
        compact_hex.reserve(hex_string.size());

        for (const char symbol: hex_string) {
            if (!std::isspace(static_cast<unsigned char>(symbol))) compact_hex.push_back(symbol);
        }

        if (compact_hex.empty()) return {};

        if (compact_hex.size() % 2 != 0) throw std::runtime_error("HEX-строка должна содержать чётное количество символов.");

        auto to_hex_value = [](const char symbol) -> std::uint8_t {
            if (symbol >= '0' && symbol <= '9') return static_cast<std::uint8_t>(symbol - '0');

            if (symbol >= 'a' && symbol <= 'f') return static_cast<std::uint8_t>(symbol - 'a' + 10);

            if (symbol >= 'A' && symbol <= 'F') return static_cast<std::uint8_t>(symbol - 'A' + 10);

            throw std::runtime_error("HEX-строка содержит недопустимые символы.");
        };

        std::vector<std::uint8_t> bytes;
        bytes.reserve(compact_hex.size() / 2);

        for (std::size_t i = 0; i < compact_hex.size(); i += 2) {
            const std::uint8_t high = to_hex_value(compact_hex[i]);
            const std::uint8_t low = to_hex_value(compact_hex[i + 1]);

            bytes.push_back(static_cast<std::uint8_t>((high << 4) | low));
        }

        return bytes;
    }

    std::string task1_aux::to_hex_string(const std::vector<std::uint8_t> &data) {
        std::ostringstream output;

        output << std::hex << std::uppercase << std::setfill('0');
        for (const auto byte: data) {
            output << std::setw(2) << static_cast<int>(byte);
        }

        return output.str();
    }

    symmetric_cipher_context::symmetric_cipher_context(
        std::shared_ptr<i_symmetric_block_cipher> algorithm,
       const cipher_mode mode,
       const padding_mode padding,
       std::vector<std::uint8_t> iv,
       std::vector<std::size_t> mode_params
    )
        : algorithm_(std::move(algorithm)),
          mode_impl_(create_mode(mode)),
          padding_impl_(create_padding(padding)),
          mode_type_(mode),
          padding_type_(padding),
          iv_(std::move(iv)),
          mode_params_(std::move(mode_params))
    {

        if (!algorithm_) throw std::runtime_error("Алгоритм шифрования не задан.");

        const std::size_t block_size = algorithm_->block_size();
        if (block_size == 0) throw std::runtime_error("Размер блока алгоритма не может быть равен нулю.");

        if (mode_type_ == cipher_mode::ECB) {
            if (!iv_.empty() && iv_.size() != block_size) throw std::runtime_error("Размер IV должен совпадать с размером блока.");
        } else iv_ = normalize_iv(iv_, block_size);
    }

    void symmetric_cipher_context::encrypt(
        const std::vector<std::uint8_t> &input,
        std::vector<std::uint8_t> &output,
        std::size_t threads_count
    ) const {
        threads_count = normalize_threads_count(threads_count);

        const std::size_t block_size = algorithm_->block_size();
        std::vector<std::uint8_t> padded = padding_impl_->pad(input, block_size);

        if (padded.size() % block_size != 0) throw std::runtime_error("Данные после набивки должны быть кратны размеру блока.");

        mode_impl_->encrypt(*algorithm_, padded, output, iv_, threads_count, mode_params_);
    }

    void symmetric_cipher_context::decrypt(
        const std::vector<std::uint8_t> &input,
        std::vector<std::uint8_t> &output,
        std::size_t threads_count
    ) const {
        threads_count = normalize_threads_count(threads_count);

        const std::size_t block_size = algorithm_->block_size();
        if (
            !input.empty()
            && input.size() % block_size != 0
        ) throw std::runtime_error("Размер шифротекста должен быть кратен размеру блока.");

        if (input.empty()) {
            output.clear();
            return;
        }

        std::vector<std::uint8_t> decrypted;
        mode_impl_->decrypt(*algorithm_, input, decrypted, iv_, threads_count, mode_params_);
        padding_impl_->unpad(decrypted, block_size);
        output = std::move(decrypted);
    }

    std::future<void> symmetric_cipher_context::encrypt(
        const std::filesystem::path &input_path,
        const std::filesystem::path &output_path,
        const std::size_t threads_count
    ) const {
        const auto algorithm = algorithm_;
        const auto mode_type = mode_type_;
        const auto padding_type = padding_type_;
        const auto iv = iv_;
        const auto mode_params = mode_params_;

        return std::async(std::launch::async,
            [algorithm, mode_type, padding_type, iv, mode_params,
            input_path, output_path, threads_count]() {
                symmetric_cipher_context local_context(algorithm, mode_type, padding_type, iv, mode_params);

                const auto input = task1_aux::read_file_bytes(input_path);

                std::vector<std::uint8_t> output;

                local_context.encrypt(input, output, threads_count);
                task1_aux::write_file_bytes(output_path, output);
        });
    }

    std::future<void> symmetric_cipher_context::decrypt(
        const std::filesystem::path &input_path,
        const std::filesystem::path &output_path,
        const std::size_t threads_count
    ) const {
        const auto algorithm = algorithm_;
        const auto mode_type = mode_type_;
        const auto padding_type = padding_type_;
        const auto iv = iv_;
        const auto mode_params = mode_params_;

        return std::async(std::launch::async,
            [algorithm, mode_type, padding_type, iv, mode_params,
                input_path, output_path, threads_count]() {
                    symmetric_cipher_context local_context(algorithm, mode_type, padding_type, iv, mode_params);

                    const auto input = task1_aux::read_file_bytes(input_path);
                    std::vector<std::uint8_t> output;

                    local_context.decrypt(input, output, threads_count);
                    task1_aux::write_file_bytes(output_path, output);
        });
    }

    void task1::run(int argc, char **argv) {
        (void) argc;
        (void) argv;

        std::cout << "Режим шифрования: 1-ECB, 2-CBC, 3-PCBC" << std::endl;
        const auto selected_mode = static_cast<cipher_mode>(read_choice("> ", 1, 3));

        std::cout << "Режим набивки: 1-Zeros, 2-ANSI X.923" << std::endl;
        const auto selected_padding = static_cast<padding_mode>(read_choice("> ", 1, 2));

        std::cout << "Введите ключ в HEX: ";
        std::string key_hex;
        std::getline(std::cin >> std::ws, key_hex);

        std::vector<std::uint8_t> key = task1_aux::parse_hex_string(key_hex);
        if (key.empty()) throw std::runtime_error("Ключ не должен быть пустым.");

        std::vector<std::uint8_t> iv;
        if (selected_mode != cipher_mode::ECB) {
            std::cout << "Введите IV в HEX (16 байт): ";

            std::string iv_hex;
            std::getline(std::cin >> std::ws, iv_hex);

            iv = task1_aux::parse_hex_string(iv_hex);
        }

        const std::size_t threads_count = read_threads_count();

        auto algorithm = std::make_shared<xor_block_cipher>();
        algorithm->set_key(key);

        symmetric_cipher_context context(algorithm, selected_mode, selected_padding, iv);

        std::cout << "Операция: 1-Шифрование, 2-Дешифрование" << std::endl;
        const int operation = read_choice("> ", 1, 2);

        std::cout << "Источник данных: 1-HEX строка, 2-Файлы" << std::endl;
        const int source = read_choice("> ", 1, 2);

        if (source == 1) {
            std::cout << "Введите данные в HEX: ";

            std::string input_hex;
            std::getline(std::cin >> std::ws, input_hex);

            const std::vector<std::uint8_t> input = task1_aux::parse_hex_string(input_hex);

            std::vector<std::uint8_t> output;
            if (operation == 1) context.encrypt(input, output, threads_count);
            else context.decrypt(input, output, threads_count);

            const std::string output_hex = task1_aux::to_hex_string(output);

            std::cout << "Результат (" << output.size() << " байт, HEX): "
                      << (output_hex.empty() ? "<empty>" : output_hex) << std::endl;

            return;
        }

        std::cout << "Путь к входному файлу: ";
        std::string input_file_path;
        std::getline(std::cin >> std::ws, input_file_path);

        std::cout << "Путь к выходному файлу: ";
        std::string output_file_path;
        std::getline(std::cin >> std::ws, output_file_path);

        std::future<void> operation_task;
        if (operation == 1) operation_task = context.encrypt(input_file_path, output_file_path, threads_count);
        else operation_task = context.decrypt(input_file_path, output_file_path, threads_count);

        operation_task.get();

        std::cout << "Операция завершена. Результат записан в файл: " << output_file_path << std::endl;
    }
}

