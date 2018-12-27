# ggint
Poor man's long integer arithmetic operations.

- Header-only
- No 3rd party libraries

Numbers are represented as array of bytes:

`N = a[0] + a[1]*256 + a[2]*256^2 + a[3]*256^3 + ...`

The main goal was to learn how [Miller-Rabin primality test](https://en.wikipedia.org/wiki/Miller–Rabin_primality_test)
works and try to generate some big prime numbers from scratch.  The code is not optimized and probably has bugs and most
likely this is not the correct way to implement efficient long integer arithmetic. Nevertheless, it manages to find
random prime and [safe prime](https://en.wikipedia.org/wiki/Safe_prime) numbers in reasonable time (although much slower
than openssl):

    $ ./find_prime 256

        Usage: ./find_prime nbits
        Generating small primes for fast sieve check
        Max prime in sieve = 4093
        Searching for 256-bit prime ...
        .........
        Found prime p:
         -                p : 131 180  43 109 186 157  78  54 244  74  36  37  75 253  52 172 251  61  58 217 167  74 241 118  74  65  52   3 103 110 254 224
                    Decimal : 101767619206415467646517001340499112030636580664836015053725702323129188856963

    $  openssl prime 101767619206415467646517001340499112030636580664836015053725702323129188856963

        101767619206415467646517001340499112030636580664836015053725702323129188856963 is prime

    $ ./find_safe_prime 192

        Usage: ./find_safe_prime nbits
        Generating small primes for fast sieve check
        Max prime in sieve = 4093
        Searching for 192-bit safe prime ...
        ......
        Found safe prime p:
         -                p :  67 150 125  76 190  74 243 119 255  95 250 199  86 185 239 251 115 251  15 193 148 232  55 211
                    Decimal : 5179059918294645595326237229226656885717064063222063928899
         -          (p-1)/2 :  33 203  62  38  95 165 249 187 255  47 253  99 171 220 247 253 185 253 135  96  74 244 155 105
                    Decimal : 2589529959147322797663118614613328442858532031611031964449

    $ openssl prime 5179059918294645595326237229226656885717064063222063928899

        5179059918294645595326237229226656885717064063222063928899 is prime

    $ openssl prime 2589529959147322797663118614613328442858532031611031964449

        2589529959147322797663118614613328442858532031611031964449 is prime
