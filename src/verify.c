// gcc verify.c -lbsd

#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

#include "disjoint.c"

static bool almostEqual(double a, double b, double e) {
  return fabs(a-b) < e;
}

int main(int argc, char * argv[]) {
  const int SIZE = 12; // TODO test odd size too
  init(SIZE);

  int track[SIZE][SIZE][SIZE];
  int count[SIZE]; // how many times with this interlace size
  memset(track, 0, sizeof(track));
  const int K = 1000000;
  for(int k = 0; k < K; k++) {
    // one full pass
    uint32_t interlace_size;
    uint32_t before[SIZE];
    memcpy(before, t, sizeof(uint32_t) * SIZE);
    for(int i = 0; i < SIZE; i++) {
      next();
      if(i == 0) interlace_size = d;
      // the interlace size is expected to be constant during the pass
      assert(d == interlace_size);
    }
    count[interlace_size]++;
    // check for duplicates
    for(int i = 0; i < SIZE; i++) {
      for(int j = i + 1; j < SIZE; j++) {
        assert(t[i] != t[j]);
      }
    }
    // track where each position ended up
    for(int was = 0; was < SIZE; was++) {
      for(int is = 0; is < SIZE; is++) {
        if(before[was] == t[is]) {
          track[interlace_size][was][is]++;
        }
      }
    }
  }
  const int h = SIZE / 2;
  for(int interlace_size = 1; interlace_size <= h / 2; interlace_size++) {
    assert(count[interlace_size] != 0);
    for(int was = 0; was < SIZE; was++) {
      for(int is = 0; is < SIZE; is++) {
        const int c = track[interlace_size][was][is];
        // is 'is' within same group as 'was', remember:
        // the first disjoint shuffle is from [0,h-d-1] and [h, h+d-1].
        const int d = interlace_size;
        const bool was_in_group_a = (was < h - d) || (was >= h && was < h + d);
        const bool is_in_group_a = (is < h - d) || (is >= h && is < h + d);
        // the destination that would be outside the respective disjoint half should be 0
        assert(was_in_group_a == is_in_group_a || c == 0);
        // all other destination for all source are expected to be roughly equivalent
        const double expected = 1 / 
          (double)(was_in_group_a? (SIZE/2) : SIZE - (SIZE/2));
        const double value = c / (double)count[interlace_size];
        const double epsilon = 0.005; // a function K, but whatever
        if(c != 0 && !almostEqual(value, expected, epsilon)) {
          // bad
          printf("BAD %d %f %f\n", c, value, expected);
        }
      }
    }
  }

  shutdown();
  return 0;
}
