// gcc stats.c -ldl -lm

#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <dlfcn.h>
#include <stdlib.h>

void init(uint32_t _n);

uint32_t next();

typedef struct {
  uint32_t n;
  uint32_t min;
  uint32_t max;
  uint64_t sum;
  uint64_t sum_squared;
} stats_t;

void stats_add(stats_t * s, uint32_t x) {
  // TODO crash on overflow
  s->sum += x;
  s->sum_squared += x * x;
  if (s->n == 0 || x < s->min) s->min = x;
  if (s->n == 0 || x > s->max) s->max = x;
  s->n++;
}

double stats_average(stats_t * s) {
  return s->sum / (double) s->n;
}

double stats_var(stats_t * s) {
  double avg = stats_average(s);
  return s->sum_squared / (double) s->n - avg * avg;
}

double stats_stddev(stats_t * s) {
  return sqrt(stats_var(s));
}

int main(int argc, char * argv[]) {
  // read args
  if(argc < 4) {
    printf("Need shared library and SIZE and STATS_COUNT. e.g. ./random.so 100 10000\n");
    return 1;
  }
  const char * sequence_so = argv[1]; // e.g. ./random.so
  const int SIZE = atoi(argv[2]); // e.g. 100
  const int STATS_COUNT = atoi(argv[3]); // e.g. 10000

  // open dynamic library
  void * sequence = dlopen(sequence_so, RTLD_NOW);
  if(!sequence) { printf("%s\n", dlerror()); return 1; }
  void (*init)(uint32_t) = dlsym(sequence, "init");
  if(!init) { printf("%s\n", dlerror()); return 1; }
  uint32_t (*next)() = dlsym(sequence, "next");
  if(!next) { printf("%s\n", dlerror()); return 1; }
  void (*shutdown)() = dlsym(sequence, "shutdown");
  if(!shutdown) { printf("%s\n", dlerror()); return 1; }

  // init sequence
  init(SIZE);

  // get the first element
  int x = next();

  // for a specific amount of tries
  stats_t stats = {0};
  for(int i = 0; i < STATS_COUNT; i++) {
    // loop until the element is found again
    int distance = 1;
    while(next() != x) distance++;
    // keep stats of how far it was
    stats_add(&stats, distance);
  }

  // shutdown sequence
  shutdown();

  // report
  double min = stats.min / (double)SIZE;
  double max = stats.max / (double)SIZE;
  double avg = stats_average(&stats) / (double)SIZE;
  double std = stats_stddev(&stats) / stats_average(&stats);
  const char * judge_min, * judge_max, * judge_avg, * judge_std;
  // not seeing the same element for a full run is optimal, but means its just the same thing over and over
  if(min > .9 && min < 1.1) judge_min = "ideal";
  // taking more than 2 runs is horrible
  // having the same element too close in a row is horrid too
  else if(min > 2 || min < .1) judge_min = "horrid";
  else judge_min = "good";
  // max has the same logic
  if(max > .9 && max < 1.1) judge_max = "ideal";
  else if(max > 2 || max < .1) judge_max = "horrid";
  else judge_max = "good";
  // avg has the same logic
  if(avg > .9 && avg < 1.1) judge_avg = "ideal";
  else if(avg > 2 || avg < .1) judge_avg = "horrid";
  else judge_avg = "good";
  // a zero stddev means its the same sequence over and over, that's bad
  if(std <= 0.05) judge_std = "horrid";
  // the higher the better
  else if(std > .4) judge_std = "ideal";
  else judge_std = "good";

  if(argc == 4) {
    printf("min: %.2f (%u) [%s]\n", min, stats.min, judge_min);
    printf("max: %.2f (%u) [%s]\n", max, stats.max, judge_max);
    printf("avg: %.2f (%.2f) [%s]\n", avg, stats_average(&stats), judge_avg);
    printf("std: %.2f (%.2f) [%s]\n", std, stats_stddev(&stats), judge_std);
  } else {
    // report with markdown table format
    printf("| Distance | Value | Normalized | Comment |\n");
    printf("|:---:|:---:|:---:|:---:|\n");
    printf("| min | %u | %.2f | %s |\n", stats.min, min, judge_min);
    printf("| max | %u | %.2f | %s |\n", stats.max, max, judge_max);
    printf("| avg | %.2f | %.2f | %s |\n", stats_average(&stats), avg, judge_avg);
    printf("| std | %.2f | %.2f | %s |\n", stats_stddev(&stats), std, judge_std);
  }
  
  // close dynamic library
  if(dlclose(sequence)) { printf("%s\n", dlerror()); return 1; }
  return 0;
}
