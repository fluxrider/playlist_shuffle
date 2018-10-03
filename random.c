// gcc -shared -fPIC -o random.so random.c -lbsd

// Returns a random element each time (no states).

// Expected results:
// min:     1 (horrid)
// max:     ∞ (horrid)
// average: N (ideal)
// stddev:  ?

#include <stdint.h>
#include <bsd/stdlib.h>

uint32_t n;

void init(uint32_t _n) {
  n = _n;
}

void shutdown() {
}

uint32_t next() {
  return arc4random_uniform(n);
}
