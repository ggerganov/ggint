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

int main() {
    srand(time(0));

    TNum num;
    ggint::zero(num);
    num[kDigits/2] = 64;
    ggint::rand(num, num);

    printf("Digit max = %d\n", ggint::kDigitMax);
    print("a", num);

    TNum num0, q, r;
    ggint::zero(num0);
    num0[0] = 0;
    num0[1] = rand()%ggint::kDigitMax;
    print("b", num0);

    ggint::add(num0, num);
    print("a+b", num);
    ggint::sub(num0, num);
    print("a", num);
    ggint::div(num0, num, q, r);
    print("a/b", q);
    print("a%b", r);
    ggint::mod(num0, num, r);
    print("a%b", r);

    ggint::mul(128, num);
    ggint::mul(2, num);
    print("a*256", num);

    ggint::shr(num);
    print("a", num);
    print("b", num0);

    TNum num1;
    ggint::mul(num0, num, num1);
    print("a*b", num1);

    return 0;
}
