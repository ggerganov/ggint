/*! \file common.h
 *  \brief Enter description here.
 *  \author Georgi Gerganov
 */

#pragma once

#include "ggint.h"

// Miller-Robin primality test
// return false: number n is composite
// return true:  number n is very likely to be a prime
//
template <std::size_t Size>
bool is_prime(const ggint::TNumTmpl<Size> & n, std::size_t trials = 0) {
    if (ggint::is_even(n)) return false;

    using TNum = ggint::TNumTmpl<Size>;

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
        trials = 3;
    }

    for (size_t i = 0; i < trials; ++i) {
        TNum a;
        {
            TNum _max = n;
            TNum _2; ggint::set(_2, 2);
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
