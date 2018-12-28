/*! \file ggint.cpp
 *  \brief Search for N-bit safe primes
 *  \author Georgi Gerganov
 */

#include <array>
#include <limits>
#include <chrono>
#include <vector>

#include "ggint.h"
#include "common.h"

const std::size_t kDigits = 128; // max num : 2^(128*8) = 2^1024
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

    int nbits = 256;
    if (argc > 1) {
        nbits = std::max(8, atoi(argv[1]));
        nbits = std::min(512, nbits);
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
    TNum n2;
    ggint::zero(n);
    std::vector<bool> to_add(smallPrimes.back());
    std::vector<std::size_t> pmod(smallPrimes.back());
    std::vector<std::size_t> pmod2(smallPrimes.back());

    printf("Searching for %d-bit safe prime ...\n", nbits);

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

            n2 = n;
            ggint::sub(_1, n2);
            ggint::shbr(n2, 1);

            if (ggint::is_even(n2)) {
                ggint::add(2, n);
                ggint::add(1, n2);
            }

            for (auto i = 0; i < smallPrimes.size(); ++i) {
                auto p = smallPrimes[i];
                std::size_t r;
                ggint::mod(p, n, r);
                pmod[i] = r;
                ggint::mod(p, n2, r);
                pmod2[i] = r;
            }
        }

        auto tCur = std::chrono::high_resolution_clock::now();

        bool do_fast = true;
        while (true) {
            for (auto i = 0; i < smallPrimes.size(); ++i) {
                if (pmod[i] == 0 || pmod2[i] == 0) {
                    do_fast = false;
                    break;
                }
            }
            if (do_fast == false) {
                ++ncheck;
                ggint::add(4, n);
                ggint::add(2, n2);

                pmod[1] += 4; pmod[1] %= 3;
                pmod2[1] += 2; pmod2[1] %= 3;
                for (auto i = 2; i < smallPrimes.size(); ++i) {
                    auto p = smallPrimes[i];
                    pmod[i] += 4;
                    if (pmod[i] >= p) pmod[i] -= p;
                    pmod2[i] += 2;
                    if (pmod2[i] >= p) pmod2[i] -= p;
                }
                do_fast = true;
                continue;
            } else {
                break;
            }
        }

        {
            //printf("ncheck = %d\n", (int) ncheck);
            tEnd = std::chrono::high_resolution_clock::now();
            //printf("Fast check: %d us\n", (int) std::chrono::duration_cast<std::chrono::microseconds>(tEnd - tCur).count());
        }

        bool is_safe_prime = true;
        if (is_prime(n, 3) && is_prime(n2, 3) && is_prime(n, nbits/16) && is_prime(n2, nbits/16)) {
        } else {
            is_safe_prime = false;
            printf(".");
            fflush(stdout);
        }

        if (is_safe_prime) {
            printf("\nFound safe prime p:\n");
            ggint::print("p", n);
            ggint::print("(p-1)/2", n2);
            break;
        }

        ++ncheck;
        ggint::add(4, n);
        ggint::add(2, n2);

        pmod[1] += 4; pmod[1] %= 3;
        pmod2[1] += 2; pmod2[1] %= 3;
        for (auto i = 2; i < smallPrimes.size(); ++i) {
            auto p = smallPrimes[i];
            pmod[i] += 4;
            if (pmod[i] >= p) pmod[i] -= p;
            pmod2[i] += 2;
            if (pmod2[i] >= p) pmod2[i] -= p;
        }

        {
            tEnd = std::chrono::high_resolution_clock::now();
            //printf("Time: %d us\n", (int) std::chrono::duration_cast<std::chrono::microseconds>(tEnd - tStart).count());
        }

    }

    {
        auto t = std::chrono::duration_cast<std::chrono::milliseconds>(tEnd - tStart).count();
        printf("Checked %d numbers in %d ms: %g num/sec\n", (int) ncheck, (int) t, 1000.0*((double)(ncheck))/t);
    }

    return 0;
}
