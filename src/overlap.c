// gcc -shared -fPIC -o overlap.so overlap.c -lbsd

// Shuffle the two half sequence, then re-shuffle an overlaping region.
// This cannot be done iteratively.

// Expected results:
// min:     N/2 (good)
// max:     3N/2 (good)
// average: N (ideal)
// stddev:  similar to disjoint

#include <stdint.h>
#include <stdlib.h>
#include <bsd/stdlib.h>

uint32_t n;
uint32_t i;
uint32_t * t;

void init(uint32_t _n) {
  n = _n;
  t = malloc(sizeof(uint32_t) * n);
  for(i = 0; i < n; i++) {
    t[i] = i;
  }
  i = 0;
}

void shutdown() {
  free(t);
}

static void swap(uint32_t i, uint32_t j) {
  uint32_t tmp = t[i];
  t[i] = t[j];
  t[j] = tmp;
}

static void shuffle(uint32_t from, uint32_t to_exclusive) {
  for(uint32_t i = from; i < to_exclusive; i++) {
    uint32_t j = i + arc4random_uniform(to_exclusive - i);
    swap(i, j);
  }
}

uint32_t next() {
  if(i == n) i = 0;
  if(i == 0) {
    shuffle(0, n / 2);
    shuffle(n / 2, n);
    shuffle(n / 4, 3 * n / 4);
  }
  return t[i++];
}
