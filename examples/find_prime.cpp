/*! \file find_prime.cpp
 *  \brief Search for N-bit prime number using Miller-Rabin primality test
 *  \author Georgi Gerganov
 */

#include <array>
#include <limits>
#include <chrono>
#include <vector>

#include "ggint.h"
#include "common.h"

const std::size_t kDigits = 256; // max num : 2^(256*8) = 2^2048
using TNum = ggint::TNumTmpl<kDigits>;

// sieve
std::vector<std::size_t> smallPrimes;

void add_prime(std::size_t n) {
    for (auto p : smallPrimes) {
        if (p*p > n) break;
        if (n%p == 0) return;
    }
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

    int nbits = 512;
    if (argc > 1) {
        nbits = std::max(8, atoi(argv[1]));
        nbits = std::min(1024, nbits);
    }

    printf("Generating small primes for fast sieve check\n");
    calc_small_primes(std::min(1 << 24, 1 << (std::min(nbits, 24) - 4)));
    printf("Max prime in sieve = %lu\n", smallPrimes.back());

    TNum n_lo, n_hi, _1;
    ggint::one(n_lo);
    ggint::one(n_hi);
    ggint::one(_1);
    ggint::shbl(n_lo, nbits - 1);
    ggint::shbl(n_hi, nbits);
    ggint::sub(n_lo, n_hi);

    TNum n;
    ggint::zero(n);
    std::vector<std::size_t> pmod(smallPrimes.back());

    printf("Searching for %d-bit prime ...\n", nbits);

    auto tStart = std::chrono::high_resolution_clock::now();
    auto tEnd = std::chrono::high_resolution_clock::now();

    std::size_t ncheck = 0;
    while (true) {
        if (ggint::is_zero(n)) {
            ggint::rand(n, n_hi);
            ggint::add(n_lo, n);
            if (ggint::is_even(n)) {
                ggint::add(1, n);
            }
            for (auto i = 0; i < smallPrimes.size(); ++i) {
                auto p = smallPrimes[i];
                std::size_t r;
                ggint::mod(p, n, r);
                pmod[i] = r;
            }
        }

        auto tCur = std::chrono::high_resolution_clock::now();

        bool do_fast = true;
        while (true) {
            for (auto i = 0; i < smallPrimes.size(); ++i) {
                if (pmod[i] == 0) {
                    do_fast = false;
                    break;
                }
            }
            if (do_fast == false) {
                ++ncheck;
                ggint::add(2, n);
                for (auto i = 0; i < smallPrimes.size(); ++i) {
                    auto p = smallPrimes[i];
                    pmod[i] += 2;
                    if (pmod[i] >= p) pmod[i] -= p;
                }
                do_fast = true;
                continue;
            } else {
                break;
            }

            if (do_fast == false) break;
        }

        {
            //printf("ncheck = %d\n", (int) ncheck);
            tEnd = std::chrono::high_resolution_clock::now();
            //printf("Fast check: %d us\n", (int) std::chrono::duration_cast<std::chrono::microseconds>(tEnd - tCur).count());
        }

        if (is_prime(n, std::max(10, nbits/16))) {
            printf("\nFound prime p:\n");
            ggint::print("p", n);
            break;
        } else {
            printf(".");
            fflush(stdout);
        }

        ++ncheck;
        ggint::add(2, n);
        for (auto i = 0; i < smallPrimes.size(); ++i) {
            auto p = smallPrimes[i];
            pmod[i] += 2;
            if (pmod[i] >= p) pmod[i] -= p;
        }
    }

    {
        auto t = std::chrono::duration_cast<std::chrono::milliseconds>(tEnd - tStart).count();
        printf("Checked %d numbers in %d ms: %g num/sec\n", (int) ncheck, (int) t, 1000.0*((double)(ncheck))/t);
    }

    return 0;
}
