#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <future>
#include <memory>
#include <string>
#include <vector>

namespace tasks {
    enum class cipher_mode {
        ECB = 1,
        CBC = 2,
        PCBC = 3
    };

    enum class padding_mode {
        Zeros = 1,
        ANSI_X923 = 2
    };

    class i_symmetric_block_cipher {
    public:
        virtual ~i_symmetric_block_cipher() = default;

        [[nodiscard]] virtual std::size_t block_size() const = 0;
        virtual void set_key(const std::vector<std::uint8_t> &key) = 0;
        virtual void encrypt_block(const std::uint8_t *input, std::uint8_t *output) const = 0;
        virtual void decrypt_block(const std::uint8_t *input, std::uint8_t *output) const = 0;
    };

    class i_cipher_mode {
    public:
        virtual ~i_cipher_mode() = default;

        virtual void encrypt(const i_symmetric_block_cipher &algorithm,
                             const std::vector<std::uint8_t> &input,
                             std::vector<std::uint8_t> &output,
                             const std::vector<std::uint8_t> &iv,
                             std::size_t threads_count,
                             const std::vector<std::size_t> &mode_params) const = 0;

        virtual void decrypt(const i_symmetric_block_cipher &algorithm,
                             const std::vector<std::uint8_t> &input,
                             std::vector<std::uint8_t> &output,
                             const std::vector<std::uint8_t> &iv,
                             std::size_t threads_count,
                             const std::vector<std::size_t> &mode_params) const = 0;
    };

    class i_padding_mode {
    public:
        virtual ~i_padding_mode() = default;

        [[nodiscard]] virtual std::vector<std::uint8_t> pad(
                const std::vector<std::uint8_t> &input,
                std::size_t block_size
            ) const = 0;

        virtual void unpad(std::vector<std::uint8_t> &input, std::size_t block_size) const = 0;
    };

    class symmetric_cipher_context {
    public:
        symmetric_cipher_context(std::shared_ptr<i_symmetric_block_cipher> algorithm,
                                 cipher_mode mode,
                                 padding_mode padding,
                                 std::vector<std::uint8_t> iv = {},
                                 std::vector<std::size_t> mode_params = {});

        void encrypt(const std::vector<std::uint8_t> &input,
                     std::vector<std::uint8_t> &output,
                     std::size_t threads_count = 1) const;

        void decrypt(const std::vector<std::uint8_t> &input,
                     std::vector<std::uint8_t> &output,
                     std::size_t threads_count = 1) const;

        std::future<void> encrypt(const std::filesystem::path &input_path,
                                  const std::filesystem::path &output_path,
                                  std::size_t threads_count = 1) const;

        std::future<void> decrypt(const std::filesystem::path &input_path,
                                  const std::filesystem::path &output_path,
                                  std::size_t threads_count = 1) const;

    private:
        std::shared_ptr<i_symmetric_block_cipher> algorithm_;
        std::unique_ptr<i_cipher_mode> mode_impl_;
        std::unique_ptr<i_padding_mode> padding_impl_;
        cipher_mode mode_type_;
        padding_mode padding_type_;
        std::vector<std::uint8_t> iv_;
        std::vector<std::size_t> mode_params_;
    };

    class task1_aux {
    public:
        static void parallel_for_blocks(std::size_t blocks_count,
                                        std::size_t threads_count,
                                        const std::function<void(std::size_t, std::size_t)> &worker);

        static std::vector<std::uint8_t> read_file_bytes(const std::filesystem::path &path);
        static void write_file_bytes(const std::filesystem::path &path,
                                     const std::vector<std::uint8_t> &bytes);

        static void xor_blocks(const std::uint8_t *left,
                               const std::uint8_t *right,
                               std::uint8_t *result,
                               std::size_t block_size);

        static std::vector<std::uint8_t> parse_hex_string(const std::string &hex_string);
        static std::string to_hex_string(const std::vector<std::uint8_t> &data);
    };
}
