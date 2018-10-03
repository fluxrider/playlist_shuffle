// gcc -shared -fPIC -o order.so order.c

// Returns the same ordered sequence over and over.

// Expected results:
// min:     N (ideal)
// max:     N (ideal)
// average: N (ideal)
// stddev:  0 (horrid)

#include <stdint.h>
#include <stdlib.h>

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

uint32_t next() {
  if(i == n) i = 0;
  return t[i++];
}
