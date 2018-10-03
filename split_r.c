// gcc -shared -fPIC -o split_r.so split_r.c -lbsd

// Shuffle the first half and second half of the sequence separately.
// The half point changes per sequence.

// Expected results:
// min:     N/2 (good)
// max:     N (ideal)
// average: N (ideal)
// stddev:  higher than split, but lower than disjoint

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
  t = malloc(sizeof(uint32_t) * n);
  for(i = 0; i < n; i++) {
    t[i] = i;
  }
  i = 0;
}

void shutdown() {
  free(t);
}

static uint32_t range(uint32_t min, uint32_t max_inclusive) {
  return min + arc4random_uniform(max_inclusive - min + 1);
}

static void swap(uint32_t i, uint32_t j) {
  uint32_t tmp = t[i];
  t[i] = t[j];
  t[j] = tmp;
}

uint32_t next() {
  if(i == N) i = 0;
  if(i == 0) {
    n1 = range(N/4, N*3/4);
    n2 = N - n1;
  }
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
