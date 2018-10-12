// gcc print.c -ldl

#include <stdint.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>

void init(uint32_t _n);

uint32_t next();

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

  // print the sequence
  for(int i = 0; i < STATS_COUNT; i++) {
    printf("%d %d\n", i, next());
  }

  // shutdown sequence
  shutdown();
 
  // close dynamic library
  if(dlclose(sequence)) { printf("%s\n", dlerror()); return 1; }
  return 0;
}
