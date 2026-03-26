#pragma once

#include <cstdint>

#include "../../hw5/task1/number_theory_service.h"

namespace tasks {
    class probabilistic_primality_test : protected number_theory_service {
    public:
        struct result {
            bool is_probably_prime;
            double achieved_probability;
            int iterations_used;
        };

        virtual ~probabilistic_primality_test() = default;
        result test(std::int64_t value, double min_probability) const;
        virtual const char *name() const = 0;

    protected:
        virtual bool run_iteration(std::int64_t value, std::int64_t witness) const = 0;
        virtual double composite_witness_probability() const = 0;
        virtual std::int64_t choose_witness(std::int64_t value) const;
    };

    class solovay_strassen_primality_test final : public probabilistic_primality_test {
    public:
        const char *name() const override;

    protected:
        bool run_iteration(std::int64_t value, std::int64_t witness) const override;
        double composite_witness_probability() const override;
    };

    class miller_rabin_primality_test final : public probabilistic_primality_test {
    public:
        const char *name() const override;

    protected:
        bool run_iteration(std::int64_t value, std::int64_t witness) const override;
        double composite_witness_probability() const override;
    };

}
