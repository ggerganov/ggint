# ggint
Poor man's long integer arithmetic operations.

- Header-only
- No 3rd party libraries

Numbers are represented as array of bytes:

`N = a[0] + a[1]*256 + a[2]*256^2 + a[3]*256^3 + ...`

The code is not optimized and probably has bugs and most likely this is not the correct way to implement efficient long
integer arithmetic. The main goal was to learn how primality tests work and try to generate some big prime numbers from
scratch.
