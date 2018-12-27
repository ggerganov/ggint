/*! \file find_prime.cpp
 *  \brief Search for N-bit prime number using Miller-Rabin primality test
 *  \author Georgi Gerganov
 */

#include <array>
#include <limits>
#include <chrono>
#include <vector>

#include "ggint.h"

const std::size_t kDigits = 128;
using TNum = ggint::TNumTmpl<kDigits>;

std::vector<std::size_t> smallPrimes;

bool is_prime(const TNum & n, std::size_t trials = 0) {
    if (ggint::is_even(n)) return false;

    TNum _1; ggint::one(_1);
    TNum n_1 = n; ggint::sub(_1, n_1);

    std::size_t s = 0;
    {
        TNum m = n_1;
        while (ggint::is_even(m)) {
            ++s;
            ggint::shbr(m, 1);
        }
    }

    TNum d;
    {
        TNum m = n_1, t = _1, r;
        ggint::shbl(t, s);
        ggint::div(t, m, d, r);
    }

    if (trials == 0) {
        trials = 10;
        trials = std::max(trials, ((n.size()/2)*ggint::kDigitBits)/4);
    }

    for (size_t i = 0; i < trials; ++i) {
        TNum a;
        {
            TNum _max = n;
            TNum _2; ggint::set(_2, 4);
            TNum _4; ggint::set(_4, 4);
            ggint::sub(_4, _max);

            ggint::rand(a, _max);
            ggint::add(_2, a);

        }

        TNum x;
        {
            auto tStart = std::chrono::high_resolution_clock::now();
            ggint::pow_mod(a, d, n, x);
            auto tEnd = std::chrono::high_resolution_clock::now();
            //printf("  pow_mod : %d us\n", (int)(std::chrono::duration_cast<std::chrono::microseconds>(tEnd - tStart).count()));
        }

        if (ggint::equal(x, _1) || ggint::equal(x, n_1)) {
            continue;
        }

        for (std::size_t r = 0; r < s - 1; ++r) {
            TNum x2, g;
            ggint::mul(x, x, x2);
            ggint::mod(n, x2, g);
            x = g;

            if (ggint::equal(x, _1)) {
                return false;
            }

            if (ggint::equal(x, n_1)) {
                break;
            }
        }

        if (ggint::equal(x, n_1) == false) {
            return false;
        }
    }

    return true;
}

void add_prime(std::size_t n) {
    for (auto p : smallPrimes) {
        if (p*p > n) break;
        if (n%p == 0) return;
    }
    printf("    - small prime : %lu\n", n);
    smallPrimes.push_back(n);
}

void calc_small_primes(std::size_t n) {
    smallPrimes.clear();
    smallPrimes.push_back(2);
    for (auto k = 3; k < n; ++k) {
        add_prime(k);
    }
}

int main(int argc, char ** argv) {
    printf("Usage: %s nbits\n", argv[0]);

    srand(time(0));

    int nbits = 256;
    if (argc > 1) {
        nbits = std::max(8, atoi(argv[1]));
        nbits = std::min(512, nbits);
    }

    printf("Generating small primes for fast sieve check\n");
    calc_small_primes(std::min(256, 1 << (std::min(nbits, 16) - 4)));

    TNum n_lo, n_hi, _1;
    ggint::one(n_lo);
    ggint::one(n_hi);
    ggint::one(_1);
    ggint::shbl(n_lo, nbits - 1);
    ggint::shbl(n_hi, nbits);
    ggint::sub(n_lo, n_hi);

    TNum n;
    ggint::zero(n);
    std::vector<bool> to_add(smallPrimes.back());

    printf("Searching for %d-bit prime ...\n", nbits);

    while (true) {
        auto tStart = std::chrono::high_resolution_clock::now();
        auto tEnd = std::chrono::high_resolution_clock::now();

        if (ggint::is_zero(n)) {
            ggint::rand(n, n_hi);
            ggint::add(n_lo, n);
        }

        if (ggint::is_even(n)) {
            ggint::add(1, n);
        }

        bool do_fast = true;
        while (true) {
            std::fill(to_add.begin(), to_add.end(), false);
            for (auto p : smallPrimes) {
                std::size_t r;
                ggint::mod(p, n, r);
                do {
                    to_add[r] = true;
                    r += p;
                } while (r < to_add.size());
            }

            for (std::size_t i = 0; i < to_add.size(); ++i) {
                if (to_add[i] == false) {
                    if (i == 0) {
                        do_fast = false;
                    } else {
                        ggint::add(i, n);
                    }
                    break;
                }
            }

            if (do_fast == false) break;
        }

        if (is_prime(n, std::max(10, nbits/4))) {
            ggint::print("Found prime", n);
            break;
        }

        ggint::add(2, n);

        {
            tEnd = std::chrono::high_resolution_clock::now();
            //printf("Time: %d us\n", (int) std::chrono::duration_cast<std::chrono::microseconds>(tEnd - tStart).count());
        }
    }

    return 0;
}
