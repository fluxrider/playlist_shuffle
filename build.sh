set -e

gcc stats.c -ldl -lm
gcc -shared -fPIC -o order.so order.c
gcc -shared -fPIC -o random.so random.c -lbsd
gcc -shared -fPIC -o shuffle.so shuffle.c -lbsd
gcc -shared -fPIC -o disjoint.so disjoint.c -lbsd
gcc -shared -fPIC -o split.so split.c -lbsd
gcc -shared -fPIC -o split_r.so split_r.c -lbsd
gcc -shared -fPIC -o overlap.so overlap.c -lbsd
