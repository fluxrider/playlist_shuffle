// gcc -shared -fPIC -o shuffle.so shuffle.c -lbsd

// Re-shuffle the sequence each time we go over it.

// Expected results:
// min:     1 (horrid)
// max:     2N (horrid)
// average: N (ideal)
// stddev:  ?

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

uint32_t next() {
  if(i == n) i = 0;
  uint32_t j = i + arc4random_uniform(n - i);
  swap(i, j);
  return t[i++];
}
