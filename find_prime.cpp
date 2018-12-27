/*! \file ggint.cpp
 *  \brief Enter description here.
 *  \author Georgi Gerganov
 */

#include <array>
#include <limits>

#include "ggint.h"

const std::size_t kDigits = 8;
using TNum = ggint::TNumTmpl<kDigits>;

void print(const char * pref, const TNum & x) {
    printf(" - %10s : ", pref);
    for (auto & d : x) {
        printf("%3d ", d);
    }
    printf("\n");
}

bool is_prime(const TNum & n) {
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

    std::size_t trials = 10;
    trials = std::max(trials, ((n.size()/2)*ggint::kDigitBits)/4);

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
        ggint::pow_mod(a, d, n, x);

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

    TNum n; ggint::set(n, 197);

    if (is_prime(n)) {
        printf("Most likely prime\n");
    } else {
        printf("Definitely not prime\n");
    }

    return 0;
}
