// gcc -shared -fPIC -o split.so split.c -lbsd

// Shuffle the first half and second half of the sequence separately.

// Expected results:
// min:     N/2 (good)
// max:     N (ideal)
// average: N (ideal)
// stddev:  low

#include <stdint.h>
#include <stdlib.h>
#include <bsd/stdlib.h>

uint32_t N;
uint32_t n1;
uint32_t n2;
uint32_t i;
uint32_t * t;

void init(uint32_t n) {
  N = n;
  n1 = n / 2;
  n2 = n - n1;
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
  if(i == N) i = 0;
  uint32_t n = n1;
  uint32_t local_i = i;
  if(i >= n1) {
    n = n2;
    local_i = i - n1;
  }
  uint32_t local_j = local_i + arc4random_uniform(n - local_i);
  uint32_t j = i >= n1? local_j + n1 : local_j;
  swap(i, j);
  return t[i++];
}
