/*! \file dlp.cpp
 *  \brief Find x such that g^x mod p = q, x < 2^31, g, p, q < 2^512
 *  \author Georgi Gerganov
 */

#include "ggint.h"

#include <thread>

const std::size_t kDigits = 128; // max num : 2^(128*8) = 2^1024
using TNum = ggint::TNumTmpl<kDigits>;

int main(int argc, char ** argv) {
    printf("Usage: %s [nthread]\n", argv[0]);

    int nthread = std::thread::hardware_concurrency();
    if (argc > 1) {
        nthread = std::atoi(argv[1]);
        if (nthread < 1 || nthread > std::thread::hardware_concurrency()) {
            nthread = std::thread::hardware_concurrency();
        }
    }

    printf("Using %d threads\n", nthread);

    // uncomment to randomize
    srand(time(0));

    // Generator g - some big prime number. Currently the following number is hardcoded:
    // Decimal : 9456746831008455759418004378492269420473170215454266509970267803020225793040242784839755418466370610382516494614870926790804542382049298332204385846382671
    std::vector<ggint::TDigit> gbytes {
         79, 80,  59,  224,  23, 133, 197, 180,  74,  18, 243, 169, 197, 145, 177, 181,  46, 105, 194,  40,  34, 208, 209, 230,
        110, 193, 227,  25, 123,  13,  19,  85,  38,  96,   8,  28,  10, 171, 252, 103,  85,  72, 192,  13, 164,  82, 137,  68,
         83, 84,  213,  77,  30,  12, 255,  80,  34,  69, 177, 134, 154, 157, 143, 180,
    };

    // Prime p - some big prime number. Currently the following number is hardcoded:
    // Decimal : 12378906059519127458310609554398266922939100031765205007867339459855295869629445866487073269538954194095797384746272202142613890509010176538034231237940969
    std::vector<ggint::TDigit> pbytes {
        233,  22, 240,  17, 213,  44,  78, 203, 210, 166, 183, 116, 151, 247, 203, 227, 116,  27, 133, 243, 187, 249,  41, 250,
        131, 142, 131, 172,  68,  89, 228,  59, 213, 120, 234,   6, 137,  32, 187,  24, 208, 245, 240,  17,   7,  15,  56, 112,
        227,  17, 140,  81,   8, 188, 254, 206, 183, 163,  49,  97,  25, 213,  90, 236,
    };

    TNum g, p, q;

    ggint::zero(g);
    for (int i = 0; i < (int) gbytes.size(); ++i) g[i] = gbytes[i];
    ggint::print("g", g, false);

    ggint::zero(p);
    for (int i = 0; i < (int) pbytes.size(); ++i) p[i] = pbytes[i];
    ggint::print("p", p, false);

    // generate x randomly and pretend we don't know it.
    // we want to find it
    uint64_t xmax = 1 << 31;
    uint64_t xtrue = rand()%xmax;

    // Number q - in real world, this number is given (i.e. we observe it during the target communication).
    // Here we generate it using the true x from above
    {
        TNum x;
        ggint::set(x, xtrue);
        ggint::pow_mod(g, x, p, q);
        ggint::print("q", q, false);
    }

    // precompute gn = g^n mod p, n - nthreads
    TNum gn = g;
    for (int i = 1; i < nthread; ++i) {
        TNum t;
        ggint::mul(gn, g, t);
        ggint::mod(p, t, gn);
    }
    ggint::print("g^nthread", gn, false);

    TNum gcur;
    ggint::set(gcur, 1);

    printf("\n");
    printf("True x = %d\n", (int) xtrue);
    printf("Searching ... please wait\n");

    bool found = false;
    std::vector<std::thread> worker(nthread);
    for (int i = 0; i < nthread; ++i) {
        TNum t;
        ggint::mul(gcur, g, t);
        ggint::mod(p, t, gcur);

        // The core search:
        // The i-th worker tests: g^i, g^(i+n), g^(i+2n), g^(i + 3n), ...
        // n is the number of threads
        // We use the equality:
        //
        //      g^(i+n) mod p = ( (g^i mod p) * (g^n mod p) ) mod p
        //
        // The number gn = (g^n mod p) is precomputed
        //
        worker[i] = std::thread([&, i, gcur]() mutable {
            uint64_t x = i + 1;
            while (x < xmax) {
                if (ggint::equal(gcur, q)) {
                    printf("\n");
                    printf("Found x = %d\n", (int) x);
                    if (x == xtrue) {
                        printf("Success! Found x and True x match\n");
                    } else {
                        printf("Failure! Found x and True x do not match\n");
                    }
                    found = true;
                }
                if (found) break;

                TNum t;
                ggint::mul(gcur, gn, t);
                ggint::mod(p, t, gcur);

                x += nthread;

                // progress
                if (i == 0 && x % 1000 < nthread) {
                    printf(".");
                    fflush(stdout);
                }
                if (i == 0 && x % 100000 < nthread) {
                    printf("\n");
                    fflush(stdout);
                }
            }
        });
    }
    for (int i = 0; i < nthread; ++i) {
        worker[i].join();
    }

    return 0;
}
