// gcc -shared -fPIC -o disjoint.so disjoint.c -lbsd

// Disjoint shuffle the sequence each time we go over it.

// Expected results:
// min:     .5N (good)
// max:     1.5N (good)
// average: N (ideal)
// stddev:  ~.4? (good)

#include <stdint.h>
#include <stdlib.h>
#include <bsd/stdlib.h>

uint32_t n;
uint32_t i;
uint32_t * t;
uint32_t d;


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

static uint32_t range(uint32_t min, uint32_t max_inclusive) {
  return min + arc4random_uniform(max_inclusive - min + 1);
}

static void swap(uint32_t i, uint32_t j) {
  uint32_t tmp = t[i];
  t[i] = t[j];
  t[j] = tmp;
}

uint32_t next() {
  uint32_t h = n / 2;
  if(i == n) i = 0;
  if(i == 0) d = range(1, h / 2);

  // the first disjoint shuffle is from [0,h-d-1] and [h, h+d-1], length is (h-d)+(d) = h.
  // the second d-shuffle is from [h-d, h-1] and [h + d, n-1], length is (d)+(n-h-d) = n - h.
  uint32_t shift_1;
  uint32_t local_n;
  uint32_t local_h = 0;
  uint32_t shift_2 = 0;
  if(i < h - d) {
    // in first half of first shuffle
    shift_1 = 0;
    local_n = h;
    local_h = h - d;
    shift_2 = d;
  } else if(i < h) {
    // in first half of second shuffle
    shift_1 = h - d;
    local_n = n - h;
    local_h = d;
    shift_2 = d;
  } else if(i < h + d) {
    // in second half of first shuffle
    shift_1 = d;
    local_n = h;
  } else {
    // in second half of second shuffle
    shift_1 = h;
    local_n = n - h;
  }
  uint32_t local_i = i - shift_1;
  uint32_t local_j = local_i + arc4random_uniform(local_n - local_i);
  if(local_j >= local_h) local_j += shift_2;
  uint32_t j = local_j + shift_1;

  swap(i, j);
  return t[i++];
}
