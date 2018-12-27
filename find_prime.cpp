/*! \file ggint.cpp
 *  \brief Enter description here.
 *  \author Georgi Gerganov
 */

#include <array>
#include <limits>
#include <chrono>

#include "ggint.h"

const std::size_t kDigits = 64;
using TNum = ggint::TNumTmpl<kDigits>;

void print(const char * pref, const TNum & x) {
    printf(" - %10s : ", pref);
    for (auto & d : x) {
        printf("%3d ", d);
    }
    printf("\n");
}

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
        auto tStart = std::chrono::high_resolution_clock::now();
        ggint::pow_mod(a, d, n, x);
        auto tEnd = std::chrono::high_resolution_clock::now();
        printf("  pow_mod : %d us\n", (int)(std::chrono::duration_cast<std::chrono::microseconds>(tEnd - tStart).count()));

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

int main() {
    //srand(time(0));
    srand(1234);

    int nbits = 64;
    TNum n_lo, n_hi, _1;
    ggint::one(n_lo);
    ggint::one(n_hi);
    ggint::one(_1);
    ggint::shbl(n_lo, nbits - 1);
    ggint::shbl(n_hi, nbits);
    ggint::sub(n_lo, n_hi);
    while (true) {
        auto tStart = std::chrono::high_resolution_clock::now();
        auto tEnd = std::chrono::high_resolution_clock::now();

        TNum n;
        ggint::rand(n, n_hi);
        ggint::add(n_lo, n);
        ggint::mul(12, n);
        ggint::sub(_1, n);

        TNum n2 = n;
        ggint::sub(_1, n2);
        ggint::shbr(n2, 1);

        bool is_safe_prime = ggint::is_odd(n2);
        for (int t = 1; is_safe_prime && t < 10; ++t) {
            if (is_prime(n, t) && is_prime(n2, t)) {
            } else {
                is_safe_prime = false;
                break;
            }
        }

        if (is_safe_prime) {
            print("safe prime", n);
        }

        tEnd = std::chrono::high_resolution_clock::now();
        printf("Time: %d us\n", (int) std::chrono::duration_cast<std::chrono::microseconds>(tEnd - tStart).count());

    }

    return 0;
}
