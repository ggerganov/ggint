/*! \file ggint.h
 *  \brief Enter description here.
 *  \author Georgi Gerganov
 */

#include <map>
#include <array>
#include <limits>
#include <random>

namespace ggint {

    using TDigit = uint8_t;
    using TOverflow = uint16_t;

    template <std::size_t Size>
        using TNumTmpl = std::array<TDigit, Size>;

    constexpr std::size_t kDigitBits = 8*sizeof(TDigit);
    constexpr TOverflow kDigitMax = (TOverflow)(std::numeric_limits<TDigit>::max()) + 1;

    // a = 0
    template<std::size_t Size>
        void zero(TNumTmpl<Size> & a) {
            a.fill(0);
        }

    // a = 1
	template<std::size_t Size>
		void one(TNumTmpl<Size> & a) {
			a.fill(0);
            a[0] = 1;
		}

    // a = n
	template<std::size_t Size>
		void set(TNumTmpl<Size> & a, std::size_t n) {
			a.fill(0);
            std::size_t i = 0;
            while (n > 0) {
                a[i] = n % kDigitMax;
                n /= kDigitMax;
            }
		}

    // b = b + a
    template<std::size_t Size>
        void add(const TNumTmpl<Size> & a, TNumTmpl<Size> & b) {
            TDigit r = 0;
            for (auto i = 0; i < Size; ++i) {
                TOverflow x = b[i];
                x += a[i];
                x += r;
                b[i] = x % kDigitMax;
                r = x / kDigitMax;
            }
        }

    // b = b + a
    template<std::size_t Size>
        void add(TDigit a, TNumTmpl<Size> & b) {
            TDigit r = 0;
            for (auto i = 0; i < Size; ++i) {
                TOverflow x = b[i];
                x += a;
                x += r;
                b[i] = x % kDigitMax;
                r = x / kDigitMax;
                a = 0;
            }
        }

    // b = b - a
    template<std::size_t Size>
        void sub(const TNumTmpl<Size> & a, TNumTmpl<Size> & b) {
            TDigit r = 0;
            for (auto i = 0; i < Size; ++i) {
                if (b[i] >= a[i] + r) {
                    b[i] -= r;
                    b[i] -= a[i];
                    r = 0;
                } else {
                    TOverflow x = kDigitMax;
                    x += b[i];
                    x -= r;
                    x -= a[i];
                    b[i] = x;
                    r = 1;
                }
            }
        }

    // shift digits left
    template<std::size_t Size>
        void shl(TNumTmpl<Size> & a, std::size_t sh = 1) {
            if (sh == 0) return;
            sh = std::min(Size, sh);
            for (auto i = Size - 1; i >= sh; --i) {
                a[i] = a[i - sh];
            }

            for (auto i = 0; i < sh; ++i) {
                a[i] = 0;
            }
        }

    // shift digits right
    template<std::size_t Size>
        void shr(TNumTmpl<Size> & a, std::size_t sh = 1) {
            if (sh == 0) return;
            sh = std::min(Size, sh);
            for (auto i = 0; i < Size - sh; ++i) {
                a[i] = a[i + sh];
            }

            for (auto i = 0; i < sh; ++i) {
                a[Size - 1 - i] = 0;
            }
        }

    // shift bits left
    template<std::size_t Size>
        void shbl(TNumTmpl<Size> & a, std::size_t sh = 1) {
            if (sh == 0) return;
            shl(a, sh / kDigitBits);
            sh = sh % kDigitBits;

            TOverflow mask = kDigitMax - (1 << (kDigitBits - sh));
            TDigit bits0 = 0, bits1 = 0;
            for (auto i = 0; i < Size; ++i) {
                bits1 = a[i] & mask;
                a[i] <<= sh;
                a[i] |= bits0 >> (kDigitBits - sh);
                bits0 = bits1;
            }
        }

    // shift bits right
    template<std::size_t Size>
        void shbr(TNumTmpl<Size> & a, std::size_t sh = 1) {
            if (sh == 0) return;
            shr(a, sh / kDigitBits);
            sh = sh % kDigitBits;

            TOverflow mask = (1 << sh) - 1;
            TDigit bits0 = 0, bits1 = 0;
            for (auto i = Size - 1; ; --i) {
                bits1 = a[i] & mask;
                a[i] >>= sh;
                a[i] |= bits0 << (kDigitBits - sh);
                bits0 = bits1;
                if (i == 0) break;
            }
        }

    // b = b * a
    template<std::size_t Size>
        void mul(TDigit a, TNumTmpl<Size> & b) {
            TDigit r = 0;
            for (auto i = 0; i < Size; ++i) {
                TOverflow x = a;
                x *= b[i];
                x += r;
                b[i] = x % kDigitMax;
                r = x / kDigitMax;
            }
        }

    // p = b * a
    template<std::size_t Size>
        void mul(const TNumTmpl<Size> & a, const TNumTmpl<Size> & b, TNumTmpl<Size> & p) {
            TNumTmpl<Size> t;
            zero(p);
            for (auto i = 0; i < Size; ++i) {
                t = b;
                mul(a[i], t);
                shl(t, i);
                add(t, p);
            }
        }

    // a == b
    template<std::size_t Size>
        bool equal(const TNumTmpl<Size> & a, const TNumTmpl<Size> & b) {
            for (auto i = 0; i < Size; ++i) {
                if (a[i] != b[i]) return false;
            }
            return true;
        }

    // a < b
    template<std::size_t Size>
        bool less(const TNumTmpl<Size> & a, const TNumTmpl<Size> & b) {
            for (auto i = Size - 1; ; --i) {
                if (a[i] < b[i]) return true;
                if (a[i] > b[i]) return false;
                if (i == 0) break;
            }
            return false;
        }

    // a <= b
    template<std::size_t Size>
        bool less_or_equal(const TNumTmpl<Size> & a, const TNumTmpl<Size> & b) {
            for (auto i = Size - 1; ; --i) {
                if (a[i] < b[i]) return true;
                if (a[i] > b[i]) return false;
                if (i == 0) break;
            }
            return true;
        }

    // a == 0
    template<std::size_t Size>
        bool is_zero(const TNumTmpl<Size> & a) {
            for (auto i = 0; i < Size; ++i) {
                if (a[i] != 0) return false;
            }
            return true;
        }

    // a & 1 == 0
    template<std::size_t Size>
        bool is_even(const TNumTmpl<Size> & a) {
            return (a[0] & 1) == 0;
        }

    // a & 1 == 1
    template<std::size_t Size>
        bool is_odd(const TNumTmpl<Size> & a) {
            return (a[0] & 1) == 1;
        }

    // b / a = q, b % a = r
    template<std::size_t Size>
        void div(const TNumTmpl<Size> & a, const TNumTmpl<Size> & b, TNumTmpl<Size> & q, TNumTmpl<Size> & r) {
            zero(q);
            zero(r);
            TNumTmpl<Size> t;

            for (auto i = Size - 1; ; --i) {
                shl(r);
                add(b[i], r);
                if (less_or_equal(a, r)) {
                    t = a;
                    TDigit k = 0;
                    do {
                        ++k;
                        add(a, t);
                    } while (less_or_equal(t, r));
                    q[i] = k;
                    sub(a, t);
                    sub(t, r);
                } else {
                    q[i] = 0;
                }
                if (i == 0) break;
            }
        }

    template<std::size_t Size>
        void print(const char * pref, const TNumTmpl<Size> & x) {
            printf(" - %10s : ", pref);
            for (auto & d : x) {
                printf("%3d ", d);
            }
            printf("\n");
        }

    // b % a = r
    template<std::size_t Size>
        void mod(const TNumTmpl<Size> & a, const TNumTmpl<Size> & b, TNumTmpl<Size> & r) {
            zero(r);
            TNumTmpl<Size> t;

            std::map<TDigit, TNumTmpl<Size>> cache;

            if (cache.empty()) {
                zero(t);
                for (int k = 0; k <= kDigitMax; ++k) {
                    cache[k] = t;
                    add(a, t);
                }
            }

            for (auto i = Size - 1; ; --i) {
                shl(r);
                add(b[i], r);

                if (less_or_equal(a, r)) {
                    if (less_or_equal(cache[kDigitMax - 1], r)) {
                        sub(cache[kDigitMax - 1], r);
                    } else {
                        int k0 = 0;
                        int k1 = kDigitMax;
                        while (true) {
                            int m = (k0 + k1)/2;
                            if (less_or_equal(cache[m], r)) {
                                k0 = m;
                            } else {
                                k1 = m;
                            }
                            if (k1 == k0 + 1) break;
                        };
                        sub(cache[k0], r);
                    }
                }
                if (i == 0) break;
            }
        }

    // generate random number a
    template<std::size_t Size>
        void rand(TNumTmpl<Size> & a) {
            for (auto & d : a) {
                d = std::rand() % kDigitMax;
            }
        }

    // generate random number a < b
    template<std::size_t Size>
        void rand(TNumTmpl<Size> & a, const TNumTmpl<Size> b) {
            for (auto & d : a) {
                d = std::rand() % kDigitMax;
            }

            auto t = a;
            TNumTmpl<Size> q;
            div(b, t, q, a);
        }

    // r = a^x mod n
    template<std::size_t Size>
        void pow_mod(TNumTmpl<Size> a, TNumTmpl<Size> x, const TNumTmpl<Size> & n, TNumTmpl<Size> & r) {
            TNumTmpl<Size> t;
            one(r);
            zero(t);

            while (is_zero(x) == false) {
                if (is_odd(x)) {
                    mul(a, r, t);
                    mod(n, t, r);
                }
                shbr(x, 1);
                mul(a, a, t);
                auto tStart = std::chrono::high_resolution_clock::now();
                mod(n, t, a);
                auto tEnd = std::chrono::high_resolution_clock::now();
                //printf("  a : %d us\n", (int)(std::chrono::duration_cast<std::chrono::microseconds>(tEnd - tStart).count()));
            }
        }

}
